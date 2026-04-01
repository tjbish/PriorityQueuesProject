#pragma once

#include "IPriorityQueue.hpp"
#include "PairingHeap.hpp"

/*
  Adapter that exposes PairingHeap through the IPriorityQueue interface.
  Stores (key, vertex) as the heap's payload and compares by key only.
*/
template <typename KeyT>
class PairingHeapAdapter : public IPriorityQueue<KeyT>
{
public:
    using Handle = typename IPriorityQueue<KeyT>::Handle;

    // Insert (key, vertex) and return an opaque handle to the node.
    Handle insert(const KeyT &key, int vertex) override
    {
        NodeHandle h = heap_.push(Item{key, vertex});
        return static_cast<Handle>(h);
    }

    // Remove and return the minimum-key item.
    PQItem<KeyT> extractMin() override
    {
        const Item &top = heap_.top();
        PQItem<KeyT> out{top.key, top.vertex};
        heap_.pop();
        return out;
    }

    // Decrease the key for the node referred to by handle.
    void decreaseKey(Handle h, const KeyT &newKey) override
    {
        NodeHandle node = static_cast<NodeHandle>(h);
        heap_.decrease_key(node, Item{newKey, node->key.vertex});
    }

    // True if heap is empty.
    bool empty() const override { return heap_.empty(); }

private:
    struct Item
    {
        KeyT key;
        int vertex;
    };

    struct ItemLess
    {
        bool operator()(const Item &a, const Item &b) const { return a.key < b.key; }
    };

    using Heap = PairingHeap<Item, ItemLess>;
    using NodeHandle = typename Heap::handle_type;

    Heap heap_;
};