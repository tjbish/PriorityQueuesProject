#pragma once

#include "Graph.hpp"
#include "IPriorityQueue.hpp"
#include <vector>
#include <limits>

// returns total weight of the MST. Graph must be undirected and connected.
template <typename KeyT>
KeyT primMST(
    const Graph& g,
    IPriorityQueue<KeyT>& pq
)
{
    const KeyT INF = std::numeric_limits<KeyT>::max();

    int n = g.numV();
    std::vector<KeyT> key(n, INF);
    std::vector<bool> inMST(n, false);
    std::vector<typename IPriorityQueue<KeyT>::Handle> handles(n, nullptr);

    //start from vertex 0
    for (int v = 0; v < n; v++)
    {
        if (v == 0)
        {
            key[v] = 0;
            handles[v] = pq.insert(0, v);
        }
        else
        {
            handles[v] = pq.insert(INF, v);
        }
    }

    KeyT totalWeight = 0;

    while (!pq.empty())
    {
        PQItem<KeyT> cur = pq.extractMin();
        int u = cur.vertex;

        if (inMST[u])
            continue;

        inMST[u] = true;
        totalWeight += cur.key;

        for (const Edge& e : g.neighbors(u))
        {
            int v = e.to;
            if (!inMST[v] && e.w < key[v])
            {
                key[v] = e.w;
                pq.decreaseKey(handles[v], e.w);
            }
        }
    }
    return totalWeight;
}
