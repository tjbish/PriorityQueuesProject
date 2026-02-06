#pragma once

#include "../graph/Graph.hpp"
#include "../pq/IPriorityQueue.hpp"
#include <vector>

/*
  Placeholder signature for Dijkstra. Implementation will be provided later.
  Returns a distance vector sized to g.numV().
*/
struct DijkstraResult
{
    std::vector<int> dist;
};

inline DijkstraResult runDijkstra(const Graph &g, int source, IPriorityQueue<int> &pq)
{
    (void)g;
    (void)source;
    (void)pq;
    return DijkstraResult{};
}
