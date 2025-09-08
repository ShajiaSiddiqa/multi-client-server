
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <chrono>
#include <arpa/inet.h>
#include <jsoncpp/json/json.h>
using namespace std;
using namespace std::chrono;

ssize_t writeNBytes(int fd, const char* buffer, size_t n) {
    size_t total = 0;
    while (total < n) {
        ssize_t sent = write(fd, buffer + total, n - total);
        if (sent <= 0) return sent;
        total += sent;
    }
    return total;
}

ssize_t readLine(int fd, string &line) {
    char ch;
    line.clear();
    while (true) {
        int n = read(fd, &ch, 1);
        if (n <= 0) return n;
        if (ch == '\n') break;
        line.push_back(ch);
    }
    return line.size();
}

int main() {
    int client_fd;
    struct sockaddr_in serv_addr;
    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    cout << "Connected to server!\n";

    while (true) {
        string job, filename, payload;
        cout << "Enter job type (count-words / extract-emails / base64-encode / compress / decompress / csv-to-tsv / base64-decode or 'exit'): ";
        getline(cin, job);
        if (job == "exit") break;

        cout << "Enter filename: ";
        getline(cin, filename);
        cout << "Enter your message: ";
        getline(cin, payload);

        // Send JSON metadata
        ostringstream oss;
        oss << "{"
            << "\"job\": \"" << job << "\", "
            << "\"filename\": \"" << filename << "\", "
            << "\"size\": " << payload.size()
            << "}\n";
        string jsonLine = oss.str();

        // Start timer before sending
        auto start_time = high_resolution_clock::now();

        writeNBytes(client_fd, jsonLine.c_str(), jsonLine.size());

        // Send raw payload
        writeNBytes(client_fd, payload.data(), payload.size());

        // Read JSON response
        string responseLine;
        if (readLine(client_fd, responseLine) <= 0) {
            cerr << "Failed to read response\n";
            break;
        }
        cout << "Server response: " << responseLine << endl;


        auto end_time = high_resolution_clock::now();
        duration<double> latency = end_time - start_time;

        cout << "Server response: " << responseLine << endl;
        cout << "Response latency: " << latency.count() << " seconds" << endl;
    }

    close(client_fd);
    return 0;
}

// filename:                            examples:   
// notes.txt →  text to count words in  eg: Hello world! This is a test file.
// emails.txt → test extract-emails     eg: Hello alice@example.com, please contact bob@company.org
// data.csv →  test csv-to-tsv          eg: Name,Age,Email
// sample.txt → for compress/decompress or base64-encode/decode 

// base64-encode    eg: Hello World
// base64-decode    eg: SGVsbG8gV29ybGQ=
// compress        eg: aaaaabbbbcccddeeeeeffff
