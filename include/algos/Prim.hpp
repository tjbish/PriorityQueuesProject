#pragma once

#include "../graph/Graph.hpp"
#include "../pq/IPriorityQueue.hpp"
#include <vector>

/*
  Placeholder signature for Prim's algorithm. Implementation will be provided later.
  Returns total weight and parent array for the MST.
*/
struct PrimResult
{
    long long totalWeight = 0;
    std::vector<int> parent;
};

inline PrimResult runPrim(const Graph &g, int start, IPriorityQueue<int> &pq)
{
    (void)g;
    (void)start;
    (void)pq;
    return PrimResult{};
}
