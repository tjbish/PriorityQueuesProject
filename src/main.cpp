#include "../include/algos/Dijkstra.hpp"
#include "../include/algos/Prim.hpp"
#include "../include/eval/MetricsWrapper.hpp"
#include "../include/graph/GraphGen.hpp"
#include "../include/pq/FibonacciHeap.hpp"
#include "../include/pq/PairingHeapAdapter.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct GraphCase
{
    std::string name;
    int n;
    int m;
    int wLo;
    int wHi;
    unsigned seed;
};

static Graph buildGraph(const GraphCase &gc)
{
    return makeConnectedRandomUndirected(gc.n, gc.m, gc.wLo, gc.wHi, gc.seed);
}

static void logHeader(std::ofstream &out)
{
    out << "PriorityQueuesProject Benchmark Log\n";
    out << "-----------------------------------\n";
}

static void logCaseStart(std::ofstream &out, const std::string &graphName, const std::string &heapName,
                         const std::string &algoName)
{
    out << "Test Case\n";
    out << "  Graph: " << graphName << "\n";
    out << "  Heap: " << heapName << "\n";
    out << "  Algorithm: " << algoName << "\n";
}

template <typename WrappedPQ>
static void logMetrics(std::ofstream &out, const WrappedPQ &pq, long long wallNs)
{
    out << "Results\n";
    out << "  WallTimeNs: " << wallNs << "\n";
    out << "  InsertCount: " << pq.insertCount() << "\n";
    out << "  ExtractMinCount: " << pq.extractMinCount() << "\n";
    out << "  DecreaseKeyCount: " << pq.decreaseKeyCount() << "\n";
    out << "  InsertTimeNs: " << pq.insertTimeNs() << "\n";
    out << "  ExtractMinTimeNs: " << pq.extractMinTimeNs() << "\n";
    out << "  DecreaseKeyTimeNs: " << pq.decreaseKeyTimeNs() << "\n";
    out << "  Status: NOT_IMPLEMENTED\n";
    out << "\n";
}

// for each GraphType (sparse, dense, heavily unordered)
// for each heap type (pairing, fibonacci)
// for each algorithm (Dijsktra, Prim)
// Run evaluation & log metrics

// THIS STRUCTURE SHOULD PROVIDE 12 UNIQUE TESTING CASES FOR COMPARISON AND EVALUATION
int main()
{
    const std::vector<GraphCase> graphCases = {
        {"Sparse", 200, 300, 1, 50, 1234},
        {"Dense", 200, 5000, 1, 50, 5678},
        {"HeavilyUnordered", 200, 1200, 1, 5000, 9012},
    };

    std::ofstream out("results.txt");
    if (!out)
    {
        std::cerr << "Failed to open results.txt for writing.\n";
        return 1;
    }

    logHeader(out);

    for (const auto &gc : graphCases)
    {
        Graph g = buildGraph(gc);

        for (int heapIndex = 0; heapIndex < 2; ++heapIndex)
        {
            const bool useFib = (heapIndex == 0);
            const std::string heapName = useFib ? "FibonacciHeap" : "PairingHeap";

            for (int algoIndex = 0; algoIndex < 2; ++algoIndex)
            {
                const bool useDijkstra = (algoIndex == 0);
                const std::string algoName = useDijkstra ? "Dijkstra" : "Prim";

                logCaseStart(out, gc.name, heapName, algoName);

                if (useFib)
                {
                    FibonacciHeap<int> heap;
                    MetricsWrapper<int, FibonacciHeap<int>> wrapped(heap);

                    auto t0 = std::chrono::high_resolution_clock::now();
                    if (useDijkstra)
                    {
                        (void)dijkstra(g, 0, wrapped);
                    }
                    else
                    {
                        (void)primMST(g, wrapped);
                    }
                    auto t1 = std::chrono::high_resolution_clock::now();
                    long long wallNs =
                        std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();

                    logMetrics(out, wrapped, wallNs);
                }
                else
                {
                    PairingHeapAdapter<int> heap;
                    MetricsWrapper<int, PairingHeapAdapter<int>> wrapped(heap);

                    auto t0 = std::chrono::high_resolution_clock::now();
                    if (useDijkstra)
                    {
                        (void)dijkstra(g, 0, wrapped);
                    }
                    else
                    {
                        (void)primMST(g, wrapped);
                    }
                    auto t1 = std::chrono::high_resolution_clock::now();
                    long long wallNs =
                        std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();

                    logMetrics(out, wrapped, wallNs);
                }
            }
        }
    }

    std::cout << "Results written to results.txt\n";
    return 0;
}
