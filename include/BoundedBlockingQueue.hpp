#ifndef BOUNDED_BLOCKING_QUEUE_HPP
#define BOUNDED_BLOCKING_QUEUE_HPP

#include <iostream>
#include <stdexcept>
#include <mutex>
#include <condition_variable>
#include <vector>
#include "IQueue.hpp"

using namespace std;

template<typename T>
class BoundedBlockingQueue : public IQueue<T>
{
private:
    vector<T> buffer;
    size_t capacity;
    size_t size;
    size_t front;
    size_t rear;

    mutex mtx;
    condition_variable not_full;
    condition_variable not_empty;

public:
    BoundedBlockingQueue() = delete;

    explicit BoundedBlockingQueue(size_t cap)
        : buffer(cap), capacity(cap), size(0), front(0), rear(0)
    {
        if (cap == 0)
            throw invalid_argument("Queue capacity must be greater than 0");
    }

    ~BoundedBlockingQueue() override = default;

    void enqueue(const T& value) override
    {
        unique_lock<mutex> lock(mtx);
        not_full.wait(lock, [this] { return size < capacity; });

        buffer[rear] = value;
        rear = (rear + 1) % capacity;
        ++size;

        not_empty.notify_one();
    }

    bool try_enqueue(const T& value) override
    {
        unique_lock<mutex> lock(mtx);
        if (size == capacity)
            return false;

        buffer[rear] = value;
        rear = (rear + 1) % capacity;
        ++size;

        not_empty.notify_one();
        return true;
    }

    T dequeue() override
    {
        unique_lock<mutex> lock(mtx);
        not_empty.wait(lock, [this] { return size > 0; });

        T val = buffer[front];
        front = (front + 1) % capacity;
        --size;

        not_full.notify_one();
        return val;
    }

    bool try_dequeue(T& out) override
    {
        unique_lock<mutex> lock(mtx);
        if (size == 0)
            return false;

        out = buffer[front];
        front = (front + 1) % capacity;
        --size;

        not_full.notify_one();
        return true;
    }

    T peek() const override
    {
        // unique_lock<mutex> lock(mtx);
        if (size == 0)
            throw underflow_error("Queue is empty: cannot peek");

        return buffer[front];
    }

    bool isEmpty() const override
    {
        // lock_guard<mutex> lock(mtx);
        return size == 0;
    }

    size_t getSize() const override
    {
        // lock_guard<mutex> lock(mtx);
        return size;
    }

    size_t getCapacity() const override
    {
        return capacity;
    }

    // Optional: Display function for debugging
    void display() const
    {
        // lock_guard<mutex> lock(mtx);
        if (size == 0) {
            cout << "Queue is empty\n";
            return;
        }

        cout << "Queue elements: ";
        for (size_t i = 0, idx = front; i < size; ++i, idx = (idx + 1) % capacity)
        {
            cout << buffer[idx] << " ";
        }
        cout << "\n";
    }
};

#endif 
