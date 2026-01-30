#include "../include/pq/FibonacciHeap.hpp"
#include <stdexcept>
#include <cmath>
#include <queue>
#include <iostream>

FibNode* FibonacciHeap::CreateNode(int val)
{
    return new FibNode(val);
}

void FibonacciHeap::Insert(int val)
{
    FibNode* CurNode = CreateNode(val);
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
        if (CurNode->Value < MinNode->Value)
        {
            MinNode = CurNode;
        }
    }
}

FibNode* FibonacciHeap::FindMin()
{
    return MinNode;
}

void FibonacciHeap::Union(FibNode* a, FibNode* b)
{
    FibNode* temp;

    (a->Prev)->Next = b;
    (b->Prev)->Next = a;
    temp = a->Prev;
    a->Prev = b->Prev;
    b->Prev = temp;
}

FibNode* FibonacciHeap::ExtractMin()
{
    //case empty heap
    if (MinNode == nullptr)
    {
        return MinNode;
    }
    FibNode* returnNode = MinNode;
    //case only element
    if (MinNode->Next == nullptr && MinNode->Child == nullptr)
    {
        MinNode = nullptr;
        Size--;
        return returnNode;
    }
    //case need to place children in root layer
    if (MinNode->Child != nullptr)
    {
        FibNode* startChild = MinNode->Child;
        FibNode* curNode = MinNode->Child;
        FibNode* nextNode;
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
    MinNode = MinNode->Next;
    Size--;
    Consolidate();
    return returnNode;
}

void FibonacciHeap::Consolidate()
{
    int ArraySize = (int)(log(Size)/log(2));
    FibNode* arr[ArraySize+1];
    for(int i = 0; i <= ArraySize; ++i)
    {
        arr[i] = nullptr;
    }
    FibNode* curNode = MinNode;
    FibNode* nextNode;
    FibNode* temp;
    FibNode* swapTemp;
    int degree;
    
    do
    {
        degree = curNode->Degree;
        while(arr[degree] != nullptr)
        {
            //std::cout << "top of second loop" << std::endl;
            temp = arr[degree];
            if (curNode->Value > temp->Value)
            {
                swapTemp = curNode;
                curNode = temp;
                temp = swapTemp;
            }
            if (temp == MinNode)
            {
                MinNode = curNode;
            }
            FibonacciLink(temp,curNode);
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
    for(int i = 0; i <= ArraySize; ++ i)
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
                if (arr[i]->Value < MinNode->Value)
                {
                    MinNode = arr[i];
                }
            }
            else
            {
                MinNode = arr[i];
            }
            //in website but probably redundant
            // if (MinNode == nullptr)
            // {
            //     MinNode = arr[i];
            // }
            // else if (arr[i]->Value < MinNode->Value)
            // {
            //     MinNode = arr[i];
            // }
        }
    }
}

void FibonacciHeap::FibonacciLink(FibNode* child, FibNode* parent)
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
void FibonacciHeap::DecreaseKey()
{

}

void FibonacciHeap::PrintHeap()
{
    std::queue<FibNode*> PrintQueue;
    FibNode* startPtr = MinNode;
    FibNode* curPtr = MinNode;
    FibNode* parentPtr;
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
    
    while(PrintQueue.empty() != true)
    {
        parentPtr = PrintQueue.front();
        PrintQueue.pop();
        if (parentPtr == nullptr && (!PrintQueue.empty()))
        {
            PrintQueue.push(nullptr);
            std:: cout << std::endl;
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
                std::cout << parentPtr->Parent->Value << ":";
            }
            std::cout << parentPtr->Value << " ";
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

int main()
{
    std::cout << "Test Start" << std::endl;
    FibonacciHeap testHeap;
    testHeap.Insert(10);
    testHeap.Insert(20);
    testHeap.Insert(3);
    testHeap.Insert(6);
    testHeap.Insert(123);
    testHeap.Insert(61);
    testHeap.Insert(62);
    testHeap.Insert(2);
    testHeap.Insert(24);
    testHeap.Insert(26);
    testHeap.Insert(271);
    testHeap.Insert(2372);
    testHeap.Insert(241234);
    testHeap.Insert(24124231);
    testHeap.Insert(24);
    testHeap.Insert(24124);
    testHeap.Insert(2123);
    testHeap.Insert(241);
    testHeap.Insert(6122);
    testHeap.Insert(2126);
    testHeap.Insert(1322);
    testHeap.Insert(21);
    std::cout << "Insert End" << std::endl;
    testHeap.PrintHeap();
    std::cout << testHeap.FindMin()->Value << std::endl;
    std::cout << testHeap.ExtractMin()->Value << std::endl;
    std::cout << "Extracted Min" << std::endl;
    testHeap.PrintHeap();
    std::cout << std::endl;
    std::cout << testHeap.FindMin()->Value << std::endl;
    std::cout << "Test End" << std::endl;

}