#ifndef QUEUES_FACTORY_HPP
#define QUEUES_FACTORY_HPP

#include <string>
#include <stdexcept>
#include "./IQueue.hpp"
#include "./StaticArrayQueue.hpp"
#include "./DynamicArrayQueue.hpp"
#include "./CircularQueue.hpp"
#include "./BoundedBlockingQueue.hpp"
using namespace std;

template <typename T>
class Factory
{
private:
    string q_type;

public:
    Factory(const std::string& type)
        : q_type(type) {}

    // Create a queue based on the type passed to constructor
    IQueue<T>* getQueue(size_t capacity = 1024)
    {
        if (q_type == "circular") {
            return new CircularQueue<T>(capacity);
        }
        else if (q_type == "static") {
            return new StaticArrayQueue<T>(capacity);
        }
        else if (q_type == "dynamic") {
            return new DynamicArrayQueue<T>(capacity);
        }
        else if (q_type == "blocking") {
            return new BoundedBlockingQueue<T>(capacity);
        }
        else {
            throw std::invalid_argument("Unknown queue type: " + q_type);
        }
    }
};

#endif // QUEUES_HPP
