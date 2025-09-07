#include <memory>
#include <cstddef> 
#include <iostream>
#include "../include/Queues.hpp"
using namespace std;

template<typename T>
void testQueue(unique_ptr<IQueue<T>> queue, const string& name) 
{
    cout << "Testing " << name << "...\n";

    try 
    {
        queue->enqueue(10);
        queue->enqueue(20);
        queue->enqueue(30);

        cout << "Peek: " << queue->peek() << "\n";

        cout << "Dequeue: " << queue->dequeue() << "\n";

        T val;
        if (queue->try_dequeue(val)) {
            cout << "Try Dequeue: " << val << "\n";
        }

        cout << "Size: " << queue->getSize() << "\n";

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
    }

    cout << "Done testing " << name << ".\n\n";
}

int mainTest() 
{
    testQueue<int>(make_unique<CircularQueue<int>>(5), "CircularQueue");
    testQueue<int>(make_unique<StaticArrayQueue<int>>(5), "StaticArrayQueue");
    testQueue<int>(make_unique<DynamicArrayQueue<int>>(2), "DynamicArrayQueue");
    testQueue<int>(make_unique<BoundedBlockingQueue<int>>(5), "BoundedBlockingQueue");

    return 0;
}
