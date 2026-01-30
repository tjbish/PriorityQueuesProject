#pragma once

#include <vector>
#include <cstddef>

/*
  Edge represents one connection from a vertex 'u' to vertex 'to' with weight 'w'.
*/
struct Edge
{
    int to = -1; // destination vertex id
    int w = 0;   // non-negative edge weight
};

/*
  Graph is a weighted graph stored as an adjacency list.
*/
class Graph
{
public:
    /*
      numV: number of vertices (0..n-1)
      directed: true for directed graphs, false for undirected graphs
    */
    Graph(int n, bool directed)
        : numV_(n), directed_(directed), adjList(static_cast<std::size_t>(n)) {}

    int numV() const { return numV_; }          // number of vertices
    bool directed() const { return directed_; } // directed vs undirected flag

    // Read-only neighbor list for vertex u
    const std::vector<Edge> &neighbors(int u) const { return adjList[static_cast<std::size_t>(u)]; }

    /*
      Add an edge u -> v with weight w.
      If undirected, also adds v -> u.
    */
    void addEdge(int u, int v, int w)
    {
        adjList[static_cast<std::size_t>(u)].push_back(Edge{v, w});
        if (!directed)
            adjList[static_cast<std::size_t>(v)].push_back(Edge{u, w});
    }

    /*
      Count edges:
      - directed: adjacency entries already represent edges
      - undirected: each edge stored twice, so divide by 2
    */
    std::size_t edgeCount() const
    {
        std::size_t sum = 0;
        for (const auto &lst : adjList)
            sum += lst.size();
        return directed_ ? sum : (sum / 2);
    }

private:
    int numV_ = 0;                          // number of vertices
    bool directed_ = false;                 // directed vs undirected
    std::vector<std::vector<Edge>> adjList; // adjList[u] = outgoing edges from u
};