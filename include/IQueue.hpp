#ifndef IQUEUE_HPP
#define IQUEUE_HPP

#include <cstddef> 
using namespace std;
template<typename T>
class IQueue 
{
public:
    virtual ~IQueue() = default;

    virtual void enqueue(const T& value) = 0;
    virtual bool try_enqueue(const T& value) = 0;

    virtual T dequeue() = 0;
    virtual bool try_dequeue(T& out) = 0;

    virtual T peek() const = 0;
    virtual bool isEmpty() const = 0;
    virtual size_t getSize() const = 0;
    virtual size_t getCapacity() const = 0; // 0 => unbounded
};

#endif
