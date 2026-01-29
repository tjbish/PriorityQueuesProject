#pragma once

/*
  PQItem is the item returned by extractMin().
  - key    : priority value used for ordering (distance/key)
  - vertex : associated vertex id
*/
template <typename KeyT>
struct PQItem
{
    KeyT key;   // priority value
    int vertex; // vertex payload
};

/*
  IPriorityQueue is the abstract priority-queue interface.

  insert returns a Handle so decreaseKey can access the exact node efficiently.
  Fibonacci and Pairing heaps naturally use pointers as handles.
*/
template <typename KeyT>
class IPriorityQueue
{
public:
    using Handle = void *; // opaque handle (heap chooses internal type)

    virtual ~IPriorityQueue() = default;

    // Insert (key, vertex) and return a handle to the internal node.
    virtual Handle insert(const KeyT &key, int vertex) = 0;

    // Remove and return the (key, vertex) pair with minimum key.
    virtual PQItem<KeyT> extractMin() = 0;

    // Decrease the key of the node referred to by handle.
    virtual void decreaseKey(Handle h, const KeyT &newKey) = 0;

    // True if there are no items in the priority queue.
    virtual bool empty() const = 0;
};
