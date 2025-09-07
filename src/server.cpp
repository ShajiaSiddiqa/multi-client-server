#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <cstdint> // For uint64_t
#include <cstdlib> // For std::stoi
#include "../include/QueuesFactory.hpp"  // Contains Factory and IQueue
using namespace std;

struct Request 
{
    uint64_t id;
    int client_fd;
    std::string job_type;
    std::vector<char> payload;
};

void workerThread(IQueue<Request>* queue, int worker_id)
{
    while(true)
    {
        Request req;
        queue->try_dequeue(req);
    }
}

int main(int argc, char* argv[]) 
{
    std::string queue_type = "static"; // default
    size_t capacity = 10;              // default
    unsigned int workers = 8;
    unsigned int PORT = 8080;
    int arg_size = argc;

    // Parse command-line arguments
    for (int i = 1; i < arg_size; ++i) 
    {
        std::string arg = argv[i];

        if (arg == "--queue" && i + 1 < arg_size) 
        {
            queue_type = argv[++i];
        }
        else if (arg == "--capacity" && i + 1 < arg_size) 
        {
            capacity = std::stoi(argv[++i]);
        }
        else if (arg == "--worker" && i + 1 < arg_size) 
        {
            workers = std::stoi(argv[++i]);
        }
        else if (arg == "--port" && i + 1 < arg_size) 
        {
            PORT = std::stoi(argv[++i]);
        }
        else 
        {
            std::cerr << "Unknown or incomplete argument: " << arg << std::endl;
            return 1;
        }
    }
    try {
        // Create the factory and queue
        Factory<Request> factory(queue_type);
        IQueue<Request>* queue = factory.getQueue(capacity);

        // Debug output
        cout << "Queue of type '" << queue_type << "' created with capacity " << queue->getCapacity() << ".\n";

        vector<thread> thread_pool;
        for (unsigned int i = 0; i < workers; ++i) 
        {
            thread_pool.emplace_back(workerThread, queue, i + 1);
        }

        // Clean up
        delete queue;
        for (auto& t : thread_pool) 
        {
            t.detach();  
        }
    } 
    catch (const std::exception& e) 
    {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
