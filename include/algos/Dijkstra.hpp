#pragma once

#include "../graph/Graph.hpp"
#include "../pq/IPriorityQueue.hpp"
#include <vector>
#include <limits>

// returns vector of shortest distances from source.

template <typename KeyT>
std::vector<KeyT> dijkstra(
    const Graph &g,
    int source,
    IPriorityQueue<KeyT> &pq)
{
    const KeyT INF = std::numeric_limits<KeyT>::max();

    int n = g.numV();
    std::vector<KeyT> dist(n, INF);
    std::vector<typename IPriorityQueue<KeyT>::Handle> handles(n, nullptr);

    // initialize
    for (int v = 0; v < n; v++)
    {
        if (v == source)
        {
            dist[v] = 0;
            handles[v] = pq.insert(0, v);
        }
        else
        {
            handles[v] = pq.insert(INF, v);
        }
    }

    // main loop
    while (!pq.empty())
    {
        PQItem<KeyT> cur = pq.extractMin();
        int u = cur.vertex;

        if (cur.key == INF)
            break; // remaining vertices unreachable

        for (const Edge &e : g.neighbors(u))
        {
            int v = e.to;
            KeyT alt = dist[u] + e.w;

            if (alt < dist[v])
            {
                dist[v] = alt;
                pq.decreaseKey(handles[v], alt);
            }
        }
    }

    return dist;
}
