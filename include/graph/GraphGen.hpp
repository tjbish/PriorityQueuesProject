#pragma once

#include "Graph.hpp"
#include <random>
#include <unordered_set>
#include <cstdint>

/*
  Graph generators for benchmarks.
  Use a seed so graphs are reproducible across heaps/algorithms/runs.
*/

// Return a random integer in [lo, hi]
inline int randInt(std::mt19937 &rng, int lo, int hi)
{
    std::uniform_int_distribution<int> dist(lo, hi);
    return dist(rng);
}

// Pack undirected edge (u,v) into 64-bit integer (u < v) for duplicate checking
inline std::uint64_t packUndirEdge(int u, int v)
{
    if (u > v)
    {
        int t = u;
        u = v;
        v = t;
    }

    return (static_cast<std::uint64_t>(static_cast<std::uint32_t>(u)) << 32) |
           static_cast<std::uint32_t>(v);
}

/*
  Create a connected undirected random weighted graph.

  Approach:
  1) Create a random spanning tree (guarantees connectivity)
  2) Add random edges until we reach m edges

  Parameters:
  - n: number of vertices
  - m: number of edges (at least n-1)
  - wLo/wHi: inclusive weight range
  - seed: RNG seed
*/
inline Graph makeConnectedRandomUndirected(int n, int m, int wLo, int wHi, unsigned seed)
{
    if (m < n - 1)
        m = n - 1;

    Graph g(n, false);
    std::mt19937 rng(seed);
    std::unordered_set<std::uint64_t> used;

    // 1) Spanning tree
    for (int v = 1; v < n; v++)
    {
        int u = randInt(rng, 0, v - 1);
        int w = randInt(rng, wLo, wHi);
        g.addEdge(u, v, w);
        used.insert(packUndirEdge(u, v));
    }

    // 2) Extra edges
    while (static_cast<int>(g.edgeCount()) < m)
    {
        int a = randInt(rng, 0, n - 1);
        int b = randInt(rng, 0, n - 1);
        if (a == b)
            continue;

        std::uint64_t key = packUndirEdge(a, b);
        if (used.count(key))
            continue;

        int w = randInt(rng, wLo, wHi);
        g.addEdge(a, b, w);
        used.insert(key);
    }

    return g;
}

/*
  Create a 2D grid graph (undirected).

  - vertices are laid out in (rows x cols)
  - edges connect right/down neighbors (optional diagonal neighbors)
*/
inline Graph makeGrid2D(int rows, int cols, int wLo, int wHi, bool diag, unsigned seed)
{
    Graph g(rows * cols, false);
    std::mt19937 rng(seed);

    // Convert (r,c) coordinate to vertex id
    auto id = [cols](int r, int c)
    { return r * cols + c; };

    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            int u = id(r, c);

            if (c + 1 < cols)
                g.addEdge(u, id(r, c + 1), randInt(rng, wLo, wHi));
            if (r + 1 < rows)
                g.addEdge(u, id(r + 1, c), randInt(rng, wLo, wHi));

            if (diag)
            {
                if (r + 1 < rows && c + 1 < cols)
                    g.addEdge(u, id(r + 1, c + 1), randInt(rng, wLo, wHi));
                if (r + 1 < rows && c - 1 >= 0)
                    g.addEdge(u, id(r + 1, c - 1), randInt(rng, wLo, wHi));
            }
        }
    }

    return g;
}

/*
  Create a directed graph intended to generate many successful relaxations (many decreaseKey calls).

  Construction:
  - Heavy backbone path i -> i+1 with large weights
  - Many shortcut edges i -> j with smaller-ish weights
*/
inline Graph makeDecreaseKeyStress(int n, int extraPerVertex, unsigned seed)
{
    Graph g(n, true);
    std::mt19937 rng(seed);

    // Heavy backbone edges
    for (int i = 0; i < n - 1; i++)
        g.addEdge(i, i + 1, 1000);

    // Shortcut edges
    for (int i = 0; i < n; i++)
    {
        for (int k = 0; k < extraPerVertex; k++)
        {
            if (i + 1 >= n)
                break;
            int j = randInt(rng, i + 1, n - 1);
            int w = randInt(rng, 1, 50) + (j - i);
            g.addEdge(i, j, w);
        }
    }

    return g;
}
