
#ifndef STATIC_ARRAY_QUEUE_HPP
#define STATIC_ARRAY_QUEUE_HPP

#include<iostream>
#include <stdexcept> 
#include"IQueue.hpp"
#include <cstddef> 

using namespace std;

template<typename T>
class StaticArrayQueue :public IQueue<T>
{
private:
    T *arr;
    size_t capacity;
    size_t size;
    size_t front;
    size_t rear;

public:
    StaticArrayQueue() = delete;
    StaticArrayQueue(int cap)
    {
        if (cap <= 0)
            throw invalid_argument("Queue capacity must be greater than 0");

        capacity = cap;
        front = rear = 0;
        size = 0;
        arr = new T[capacity];
    }
    StaticArrayQueue(const StaticArrayQueue& other)
        :capacity(other.capacity),size(other.size), front(other.front), rear(other.rear)
    {
        arr = new T[capacity];
        for (int i = front; i < rear; ++i) 
        {
            arr[i] = other.arr[i];
        }
    }
    ~StaticArrayQueue() override
    {
        if (arr != nullptr)
            delete[] arr;
    }
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
    bool isFull() const 
    {
        return capacity == rear;
    }
    size_t getRear() const
    {
        return rear;
    }
    size_t getFront() const
    {
        return front;
    }
    void enqueue(const T& val) override
    {
        if (isFull())
            throw overflow_error("Queue overflow: cannot enqueue, queue is full");
        
        arr[rear++] = val;
        size++;
    }
    T dequeue() override
    {
        if (isEmpty())
            throw underflow_error("Queue underflow: cannot dequeue, queue is empty");

        size--;
        return this->arr[front++];
    }
    bool try_enqueue(const T& val) override 
    {
        if (isFull())
            return false;
        arr[rear++] = val;
        size++;
        return true;
    }

    bool try_dequeue(T& out) override 
    {
        if (isEmpty())
            return false;
        out = arr[front++];
        size--;
        return true;
    }
    T peek() const override
    {
        if (isEmpty()) 
            throw underflow_error("Queue is empty: cannot peek");

        return arr[front];
    }
    void display() const 
    {
        if (isEmpty()) 
        {
            cout << "Queue is empty\n";
            return;
        }
        cout << "Queue elements: ";
        for (int i = front; i < rear; ++i)
        {
            cout << arr[i] << " ";
        }
        cout << "\n";
    }
    StaticArrayQueue& operator=(const StaticArrayQueue& other)
    {
        if(this ==  &other)
            return *this; 

        delete[] arr;

        this->capacity = other.capacity;
        this->size = other.size;
        this->front = other.front;
        this->rear = other.rear;
        arr = new T[capacity];
        for (size_t i = front; i < rear; ++i) 
        {
            arr[i] = other.arr[i];
        }

        return *this;
    }

};


#endif
