
#ifndef DYNAMIC_ARRAY_QUEUE_HPP
#define DYNAMIC_ARRAY_QUEUE_HPP

#include <vector>
#include<iostream>
#include <stdexcept> 
#include"IQueue.hpp"
#include <cstddef> 

using namespace std;

template<typename T>
class DynamicArrayQueue :public IQueue<T>
{
private:
    vector<T> buffer;
    size_t front;
    size_t rear;
    size_t size;
    size_t capacity;

    void Resize()
    {
        capacity *=2;
        vector<T> newBuffer(capacity);
        for (size_t i = 0; i < size; ++i) 
        {
            newBuffer[i] = buffer[(front + i) % capacity];
        }
        buffer = move(newBuffer);
        front = 0;
        rear = size;
    }

public:
    DynamicArrayQueue(size_t cap)
        : buffer(cap), front(0), rear(0), size(0), capacity(cap) {}

    bool isEmpty() const override
    { 
        return size == 0; 
    }

    size_t getSize() const override
    { 
        return this->size; 
    }
    size_t getCapacity() const override
    {
        return this->capacity;
    }
    size_t getRear() const
    {
        return this->rear;
    }
    size_t getFront() const
    {
        return this->front;
    }
    T peek() const override
    {
        if (isEmpty()) 
            throw underflow_error("Queue is empty");

        return this->buffer[front];
    }
    void enqueue(const T& val) override
    {
        if (size == capacity)
        {
            Resize();
        }

        buffer[rear] = val;
        rear = (rear + 1) % capacity;
        ++size;
    }
    bool try_enqueue(const T& val) override
    {
        if (size == capacity)
            return false;

        buffer[rear] = val;
        rear = (rear + 1) % capacity;
        ++size;
        return true;
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
            throw underflow_error("Dynamic Array Queue is Empty! .... ");

        cout << "Queue elements: ";
        for (size_t i = 0; i < size; ++i) 
        {
            std::cout << buffer[(front + i) % capacity] << " ";
        }
        std::cout << "\n";
    }
    ~DynamicArrayQueue() override = default;

};

#endif