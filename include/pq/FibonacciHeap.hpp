#pragma once
#include "IPriorityQueue.hpp"
#include <cmath>
#include <iostream>
#include <queue>
#include <stdexcept>

template <typename KeyT>
class FibNode
{
public:
    FibNode *Parent = nullptr;
    FibNode *Child = nullptr;
    FibNode *Next = nullptr;
    FibNode *Prev = nullptr;
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
    using Handle = void *;
    FibNode<KeyT> *MinNode = nullptr;
    int Size = 0;

public:
    FibNode<KeyT> *CreateNode(KeyT k, int v);
    FibNode<KeyT> *FindMin();
    void Union(FibNode<KeyT> *a, FibNode<KeyT> *b);
    void Consolidate();
    void FibonacciLink(FibNode<KeyT> *child, FibNode<KeyT> *parent);
    void PrintHeap();
    void Cut(FibNode<KeyT> *child, FibNode<KeyT> *parent);
    void CascadeCut(FibNode<KeyT> *curNode);
    PQItem<KeyT> NodeToPQItem(FibNode<KeyT> *node);
    // IPriorityQueue functions
    bool empty() const override;
    Handle insert(const KeyT &key, int vertex);
    void decreaseKey(Handle h, const KeyT &newKey);
    PQItem<KeyT> extractMin();
};

// Header-only implementation
template <typename KeyT>
FibNode<KeyT> *FibonacciHeap<KeyT>::CreateNode(KeyT k, int v)
{
    return new FibNode<KeyT>(k, v);
}

template <typename KeyT>
typename FibonacciHeap<KeyT>::Handle FibonacciHeap<KeyT>::insert(const KeyT &key, int vertex)
{
    FibNode<KeyT> *CurNode = CreateNode(key, vertex);
    Size++;
    if (MinNode == nullptr)
    {
        MinNode = CurNode;
        CurNode->Next = CurNode;
        CurNode->Prev = CurNode;
    }
    else
    {
        CurNode->Prev = MinNode->Prev;
        CurNode->Next = MinNode;
        (MinNode->Prev)->Next = CurNode;
        MinNode->Prev = CurNode;
        if (CurNode->Key < MinNode->Key)
        {
            MinNode = CurNode;
        }
    }
    return CurNode;
}

template <typename KeyT>
FibNode<KeyT> *FibonacciHeap<KeyT>::FindMin()
{
    return MinNode;
}

template <typename KeyT>
void FibonacciHeap<KeyT>::Union(FibNode<KeyT> *a, FibNode<KeyT> *b)
{
    FibNode<KeyT> *temp;

    (a->Prev)->Next = b;
    (b->Prev)->Next = a;
    temp = a->Prev;
    a->Prev = b->Prev;
    b->Prev = temp;
}

template <typename KeyT>
PQItem<KeyT> FibonacciHeap<KeyT>::NodeToPQItem(FibNode<KeyT> *node)
{
    if (node == nullptr)
    {
        throw std::runtime_error("extractMin() on empty heap");
    }
    PQItem<KeyT> Item;
    Item.key = node->Key;
    Item.vertex = node->Vertex;
    return Item;
}

template <typename KeyT>
PQItem<KeyT> FibonacciHeap<KeyT>::extractMin()
{
    // case empty heap
    if (MinNode == nullptr)
    {
        throw std::runtime_error("extractMin() on empty heap");
    }
    FibNode<KeyT> *z = MinNode;

    // If z has children, splice the child list into the root list.
    if (z->Child != nullptr)
    {
        FibNode<KeyT> *child = z->Child;
        FibNode<KeyT> *childCur = child;
        do
        {
            childCur->Parent = nullptr;
            childCur = childCur->Next;
        } while (childCur != child);

        // Splice circular lists: (z->Prev <-> z) and (child->Prev <-> child)
        FibNode<KeyT> *zPrev = z->Prev;
        FibNode<KeyT> *childPrev = child->Prev;

        zPrev->Next = child;
        child->Prev = zPrev;

        childPrev->Next = z;
        z->Prev = childPrev;
    }

    // Remove z from root list.
    z->Prev->Next = z->Next;
    z->Next->Prev = z->Prev;

    Size--;
    if (Size == 0)
    {
        MinNode = nullptr;
        return NodeToPQItem(z);
    }

    MinNode = z->Next;
    Consolidate();
    return NodeToPQItem(z);
}

template <typename KeyT>
void FibonacciHeap<KeyT>::Consolidate()
{
    if (Size <= 1 || MinNode == nullptr)
    {
        return;
    }
    const int maxDegree = static_cast<int>(std::log2(Size)) + 2;
    std::vector<FibNode<KeyT> *> table(static_cast<std::size_t>(maxDegree), nullptr);

    // Collect current roots to avoid iterator corruption while linking.
    std::vector<FibNode<KeyT> *> roots;
    FibNode<KeyT> *cur = MinNode;
    do
    {
        roots.push_back(cur);
        cur = cur->Next;
    } while (cur != MinNode);

    // Detach each root into a single-node circular list.
    for (FibNode<KeyT> *x : roots)
    {
        x->Prev = x;
        x->Next = x;
    }

    for (FibNode<KeyT> *x : roots)
    {
        int d = x->Degree;
        while (true)
        {
            if (d >= static_cast<int>(table.size()))
            {
                table.resize(static_cast<std::size_t>(d) + 1, nullptr);
            }
            if (table[static_cast<std::size_t>(d)] == nullptr)
            {
                table[static_cast<std::size_t>(d)] = x;
                break;
            }

            FibNode<KeyT> *y = table[static_cast<std::size_t>(d)];
            table[static_cast<std::size_t>(d)] = nullptr;
            if (x->Key > y->Key)
            {
                FibNode<KeyT> *tmp = x;
                x = y;
                y = tmp;
            }
            FibonacciLink(y, x);
            d++;
        }
    }

    MinNode = nullptr;
    for (FibNode<KeyT> *node : table)
    {
        if (node == nullptr)
        {
            continue;
        }
        node->Prev = node;
        node->Next = node;
        if (MinNode == nullptr)
        {
            MinNode = node;
        }
        else
        {
            node->Prev = MinNode->Prev;
            node->Next = MinNode;
            MinNode->Prev->Next = node;
            MinNode->Prev = node;
            if (node->Key < MinNode->Key)
            {
                MinNode = node;
            }
        }
    }
}

template <typename KeyT>
void FibonacciHeap<KeyT>::FibonacciLink(FibNode<KeyT> *child, FibNode<KeyT> *parent)
{
    (child->Prev)->Next = child->Next;
    (child->Next)->Prev = child->Prev;

    child->Prev = child;
    child->Next = child;
    child->Parent = parent;
    if (parent->Child == nullptr)
    {
        parent->Child = child;
    }
    child->Next = parent->Child;
    child->Prev = (parent->Child)->Prev;
    ((parent->Child)->Prev)->Next = child;
    (parent->Child)->Prev = child;
    parent->Degree++;
}

template <typename KeyT>
void FibonacciHeap<KeyT>::PrintHeap()
{
    std::queue<FibNode<KeyT> *> PrintQueue;
    FibNode<KeyT> *startPtr = MinNode;
    FibNode<KeyT> *curPtr = MinNode;
    FibNode<KeyT> *parentPtr;
    do
    {
        if (curPtr == nullptr)
        {
            break;
        }
        PrintQueue.push(curPtr);
        curPtr = curPtr->Next;
    } while (startPtr != curPtr);

    PrintQueue.push(nullptr);

    while (PrintQueue.empty() != true)
    {
        parentPtr = PrintQueue.front();
        PrintQueue.pop();
        if (parentPtr == nullptr && (!PrintQueue.empty()))
        {
            PrintQueue.push(nullptr);
            std::cout << std::endl;
            continue;
        }
        else if (parentPtr == nullptr)
        {
            std::cout << std::endl;
        }
        else
        {
            if (parentPtr->Parent != nullptr)
            {
                std::cout << parentPtr->Parent->Key << ":";
            }
            std::cout << parentPtr->Key << " ";
            if (parentPtr->Child != nullptr)
            {
                curPtr = parentPtr->Child;
                startPtr = parentPtr->Child;
                do
                {
                    if (curPtr == nullptr)
                    {
                        break;
                    }
                    PrintQueue.push(curPtr);
                    curPtr = curPtr->Next;
                } while (curPtr != startPtr);
            }
        }
    }
}

template <typename KeyT>
bool FibonacciHeap<KeyT>::empty() const
{
    return (Size == 0);
}

template <typename KeyT>
void FibonacciHeap<KeyT>::Cut(FibNode<KeyT> *child, FibNode<KeyT> *parent)
{
    if (child == child->Next)
    {
        parent->Child = nullptr;
    }

    child->Prev->Next = child->Next;
    child->Next->Prev = child->Prev;

    if (child == parent->Child)
    {
        parent->Child = child->Next;
    }

    parent->Degree--;
    (MinNode->Prev)->Next = child;
    child->Next = MinNode;
    child->Prev = MinNode->Prev;
    MinNode->Prev = child;
    child->Parent = nullptr;
    child->Mark = 'b';
}

template <typename KeyT>
void FibonacciHeap<KeyT>::CascadeCut(FibNode<KeyT> *curNode)
{
    FibNode<KeyT> *temp = curNode->Parent;
    if (temp != nullptr)
    {
        if (temp->Mark == 'w')
        {
            temp->Mark = 'b';
        }
        else
        {
            Cut(curNode, temp);
            CascadeCut(temp);
        }
    }
}

template <typename KeyT>
void FibonacciHeap<KeyT>::decreaseKey(Handle h, const KeyT &newKey)
{
    FibNode<KeyT> *updateNode = (FibNode<KeyT> *)h;
    if (updateNode == nullptr)
    {
        throw std::runtime_error("decreaseKey(null handle)");
    }
    if (newKey > updateNode->Key)
    {
        throw std::runtime_error("decreaseKey called with larger key");
    }
    updateNode->Key = newKey;
    FibNode<KeyT> *Parent = updateNode->Parent;
    if (Parent != nullptr && updateNode->Key < Parent->Key)
    {
        Cut(updateNode, Parent);
        CascadeCut(Parent);
    }
    if (updateNode->Key < MinNode->Key)
    {
        MinNode = updateNode;
    }
}
