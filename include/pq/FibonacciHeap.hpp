class FibNode
{
    public:
        FibNode* Parent = nullptr;
        FibNode* Child = nullptr;
        FibNode* Next = nullptr;
        FibNode* Prev = nullptr;
        int Value;
        int Degree;
        FibNode(int val)
        {
            Value = val;
            Degree = 0;
        }
};

class FibonacciHeap
{
    FibNode* MinNode = nullptr;
    int Size = 0;
    public:
        FibNode* CreateNode(int val);
        void Insert(int val);
        FibNode* FindMin();
        void Union(FibNode* a, FibNode* b);
        FibNode* ExtractMin();
        void Consolidate();
        void FibonacciLink(FibNode* child, FibNode* parent);
        void DecreaseKey();
        void PrintHeap();
};

