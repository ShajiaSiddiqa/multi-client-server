#ifndef CIRCULAR_QUEUE_HPP
#define CIRCULAR_QUEUE_HPP

#include <iostream>
#include <stdexcept>
#include "IQueue.hpp"
#include <cstddef> 
using namespace std;

template<typename T>
class CircularQueue : public IQueue<T>
{
private:
    T* buffer;
    size_t front;
    size_t rear;
    size_t capacity;
    size_t size;

public:
    CircularQueue() = delete;

    CircularQueue(size_t cap)
        : front(0), rear(0), capacity(cap), size(0)
    {
        if (cap <= 0)
            throw invalid_argument("Queue capacity must be greater than 0");

        buffer = new T[capacity];
    }

    CircularQueue(const CircularQueue& other)
        : capacity(other.capacity), front(other.front),
          size(other.size), rear(other.rear)
    {
        buffer = new T[capacity];
        for (size_t i = 0; i < capacity; ++i)
            buffer[i] = other.buffer[i];
    }

    ~CircularQueue() override
    {
        delete[] buffer;
    }

    bool isEmpty() const override
    {
        return size == 0;
    }

    bool isFull() const 
    {
        return size == capacity;
    }

    size_t getSize() const override
    {
        return this->size;
    }

    size_t getCapacity() const override
    {
        return this->capacity;
    }

    T peek() const override
    {
        if (isEmpty())
            throw underflow_error("Queue is empty");

        return buffer[front];
    }

    void enqueue(const T& val) override
    {
        if (isFull())
            throw overflow_error("Queue is full");

        buffer[rear] = val;
        rear = (rear + 1) % capacity;
        ++size;
    }

    T dequeue() override
    {
        if (isEmpty())
            throw underflow_error("Queue is empty");

        T value = buffer[front];
        front = (front + 1) % capacity;
        --size;
        return value;
    }

    bool try_enqueue(const T& val) override
    {
        if (isFull())
            return false;

        buffer[rear] = val;
        rear = (rear + 1) % capacity;
        ++size;
        return true;
    }

    bool try_dequeue(T& out) override
    {
        if (isEmpty())
            return false;

        out = buffer[front];
        front = (front + 1) % capacity;
        --size;
        return true;
    }

    void display() const
    {
        if (isEmpty())
        {
            cout << "Queue is empty\n";
            return;
        }

        cout << "Queue elements: ";
        for (int i = 0; i < size; ++i)
        {
            int idx = (front + i) % capacity;
            cout << buffer[idx] << " ";
        }
        cout << "\n";
    }

    CircularQueue& operator=(const CircularQueue& other)
    {
        if (this == &other)
            return *this;

        delete[] buffer;

        capacity = other.capacity;
        front = other.front;
        rear = other.rear;
        size = other.size;

        buffer = new T[capacity];
        for (size_t i = 0; i < capacity; ++i)
            buffer[i] = other.buffer[i];

        return *this;
    }
};

#endif
