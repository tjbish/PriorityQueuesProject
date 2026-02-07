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
    FibNode<KeyT> *returnNode = MinNode;
    // case only element
    if (MinNode->Next == MinNode && MinNode->Child == nullptr)
    {
        MinNode = nullptr;
        Size--;
        return NodeToPQItem(returnNode);
    }
    // case need to place children in root layer
    if (MinNode->Child != nullptr)
    {
        FibNode<KeyT> *startChild = MinNode->Child;
        FibNode<KeyT> *curNode = MinNode->Child;
        FibNode<KeyT> *nextNode;
        do
        {
            nextNode = curNode->Next;

            (MinNode->Prev)->Next = curNode;
            curNode->Next = MinNode;
            curNode->Prev = MinNode->Prev;
            MinNode->Prev = curNode;

            curNode->Parent = nullptr;
            curNode = nextNode;
        } while (nextNode != startChild);

        (curNode->Prev)->Next = nullptr;
    }

    (MinNode->Prev)->Next = MinNode->Next;
    (MinNode->Next)->Prev = MinNode->Prev;
    Size--;
    if (Size == 0)
    {
        MinNode = nullptr;
        return NodeToPQItem(returnNode);
    }
    MinNode = MinNode->Next;
    Consolidate();
    return NodeToPQItem(returnNode);
}

template <typename KeyT>
void FibonacciHeap<KeyT>::Consolidate()
{
    if (Size <= 1 || MinNode == nullptr)
    {
        return;
    }
    int ArraySize = (int)(log(Size) / log(2));
    FibNode<KeyT> *arr[ArraySize + 1];
    for (int i = 0; i <= ArraySize; ++i)
    {
        arr[i] = nullptr;
    }
    FibNode<KeyT> *curNode = MinNode;
    FibNode<KeyT> *temp;
    FibNode<KeyT> *swapTemp;
    int degree;

    do
    {
        degree = curNode->Degree;
        while (arr[degree] != nullptr)
        {
            temp = arr[degree];
            if (curNode->Key > temp->Key)
            {
                swapTemp = curNode;
                curNode = temp;
                temp = swapTemp;
            }
            if (temp == MinNode)
            {
                MinNode = curNode;
            }
            FibonacciLink(temp, curNode);
            if (curNode->Next == curNode)
            {
                MinNode = curNode;
            }
            arr[degree] = nullptr;
            degree++;
        }
        arr[degree] = curNode;
        curNode = curNode->Next;
    } while (curNode != MinNode);

    MinNode = nullptr;
    for (int i = 0; i <= ArraySize; ++i)
    {
        if (arr[i] != nullptr)
        {
            arr[i]->Next = arr[i];
            arr[i]->Prev = arr[i];
            if (MinNode != nullptr)
            {
                (MinNode->Prev)->Next = arr[i];
                arr[i]->Next = MinNode;
                arr[i]->Prev = MinNode->Prev;
                MinNode->Prev = arr[i];
                if (arr[i]->Key < MinNode->Key)
                {
                    MinNode = arr[i];
                }
            }
            else
            {
                MinNode = arr[i];
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
