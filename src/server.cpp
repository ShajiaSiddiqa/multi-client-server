
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <cstdint>
#include <cstdlib>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <mutex>
#include <sstream>
#include <atomic>
#include <csignal>
#include <jsoncpp/json/json.h>
#include "../include/QueuesFactory.hpp"
#include "../include/jobs.hpp"
using namespace std;

#include <chrono>
#include <fstream>

// ---------------- Globals ----------------
atomic<bool> running(true);  // server running flag

// ---------------- Helpers ----------------
ssize_t writeNBytes(int fd, const char* buffer, size_t n) {
    size_t total = 0;
    while (total < n) {
        ssize_t written = write(fd, buffer + total, n - total);
        if (written <= 0) return written;
        total += written;
    }
    return total;
}

ssize_t readNBytes(int fd, char* buffer, size_t n) {
    ssize_t total = 0;
    while (total < (ssize_t)n) {
        ssize_t bytesRead = read(fd, buffer + total, n - total);
        if (bytesRead <= 0) return bytesRead;
        total += bytesRead;
    }
    return total;
}

// ---------------- Signal Handler ----------------
void handle_sigint(int sig) {
    running = false;
    cout << "\n[Server] Shutdown signal received.\n";
}

// ---------------- Worker Thread ----------------
void workerThread(IQueue<Request>* queue, int worker_id) {
    while (running) {
        Request req;
        if (queue->try_dequeue(req)) {
            File file;
            file.content = new char[req.payload.size()];
            memcpy(file.content, req.payload.data(), req.payload.size());
            file.size = req.payload.size();
            file.job_type = strdup(req.job_type.c_str());

            auto result = JobProcessor::process(file);

            delete[] file.content;
            free(file.job_type);

            // Send JSON response
            ostringstream oss;
            oss << "{"
                << "\"id\": " << req.id << ", "
                << "\"status\": \"ok\", "
                << "\"result\": \"" << result.result << "\""
                << "}\n";
            string jsonResponse = oss.str();
            writeNBytes(req.client_fd, jsonResponse.c_str(), jsonResponse.size());
        } else {
            this_thread::sleep_for(chrono::milliseconds(10));
        }
    }
}

// ---------------- Client Handler ----------------
void clientHandler(int client_fd, IQueue<Request>* queue, uint64_t& request_id) {
    char ch;
    while (running) {
        string jsonLine;
        while (true) {
            int n = read(client_fd, &ch, 1);
            if (n <= 0) { close(client_fd); return; }
            if (ch == '\n') break;
            jsonLine.push_back(ch);
        }

        // Parse JSON
        Json::CharReaderBuilder builder;
        Json::Value root;
        string errs;
        istringstream s(jsonLine);
        if (!Json::parseFromStream(builder, s, &root, &errs)) {
            cerr << "[Server] Failed to parse JSON: " << errs << endl;
            continue;
        }

        string job = root["job"].asString();
        size_t size = root["size"].asUInt();

        vector<char> payload(size);
        if (readNBytes(client_fd, payload.data(), size) != (ssize_t)size) {
            cerr << "[Server] Failed to read full payload\n";
            close(client_fd);
            return;
        }

        // Enqueue request
        Request req;
        req.id = ++request_id;
        req.client_fd = client_fd;
        req.job_type = job;
        req.payload = move(payload);

        if (!queue->try_enqueue(req)) {
            string reject = "{ \"id\": -1, \"status\": \"error\", \"result\": \"Server busy\" }\n";
            writeNBytes(client_fd, reject.c_str(), reject.size());
        }
    }
}

// ---------------- Main ----------------
int main(int argc, char* argv[]) {
    signal(SIGINT, handle_sigint);  // handle Ctrl+C

    string queue_type = "static";
    size_t capacity = 10;
    unsigned int workers = 4;
    unsigned int PORT = 8080;

  
    // Parse command-line arguments
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--queue" && i + 1 < argc) queue_type = argv[++i];
        else if (arg == "--capacity" && i + 1 < argc) capacity = stoi(argv[++i]);
        else if (arg == "--worker" && i + 1 < argc) workers = stoi(argv[++i]);
        else if (arg == "--port" && i + 1 < argc) PORT = stoi(argv[++i]);
        else { cerr << "Unknown argument: " << arg << endl; return 1; }
    }

    Factory<Request> factory(queue_type);
    IQueue<Request>* queue = factory.getQueue(capacity);
    cout << "[Server] Queue type: " << queue_type 
         << " with capacity " << queue->getCapacity() << endl;

    // Start worker threads
    vector<thread> thread_pool;
    for (unsigned int i = 0; i < workers; ++i)
        thread_pool.emplace_back(workerThread, queue, i + 1);
    for (auto& t : thread_pool) t.detach();

    // Setup server socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 6);
    cout << "[Server] Listening on port " << PORT << " with " << workers << " workers...\n";

    uint64_t request_id = 0;
    

    while (running) {
        int new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if (new_socket < 0) continue;
        thread(clientHandler, new_socket, queue, ref(request_id)).detach();
    }

    cout << "[Server] Shutting down...\n";
    delete queue;
    close(server_fd);
    return 0;
}
