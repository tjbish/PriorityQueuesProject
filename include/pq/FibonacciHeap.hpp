#pragma once
#include "IPriorityQueue.hpp"

template <typename KeyT>
class FibNode
{
    public:
        FibNode* Parent = nullptr;
        FibNode* Child = nullptr;
        FibNode* Next = nullptr;
        FibNode* Prev = nullptr;
        KeyT Key;
        int Vertex;
        int Degree;
        char Mark;
        FibNode(KeyT k, int v)
        {
            Key = k;
            Vertex = v;
            Degree = 0;
            Mark = 'w';
        }
};

template <typename KeyT>
class FibonacciHeap : public IPriorityQueue<KeyT>
{
    using Handle = void*;
    FibNode<KeyT>* MinNode = nullptr;
    int Size = 0;
    public:
        FibNode<KeyT>* CreateNode(KeyT k, int v);
        FibNode<KeyT>* FindMin();
        void Union(FibNode<KeyT>* a, FibNode<KeyT>* b);
        void Consolidate();
        void FibonacciLink(FibNode<KeyT>* child, FibNode<KeyT>* parent);
        void PrintHeap();
        void Cut(FibNode<KeyT>* child, FibNode<KeyT>* parent);
        void CascadeCut(FibNode<KeyT>* curNode);
        PQItem<KeyT> NodeToPQItem(FibNode<KeyT>* node);
        //IPriorityQueue functions
        bool empty() const override;
        Handle insert(const KeyT &key, int vertex);
        void decreaseKey(Handle h, const KeyT &newKey);
        PQItem<KeyT> extractMin();
};
