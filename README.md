# PriorityQueuesProject

This project benchmarks two priority queue implementations under identical graph-algorithm workloads:

- `FibonacciHeap`
- `PairingHeap` (via `PairingHeapAdapter`)

Algorithms exercised:

- Dijkstra shortest paths
- Prim minimum spanning tree (MST)

The benchmark harness runs a fixed matrix of graph/heap/algorithm combinations and writes structured output to `results.txt`.

## Current Repository Layout

The codebase is currently organized with a root-level entrypoint:

```text
PriorityQueuesProject/
  main.cpp
  README.md
  CodePipeline.txt
  results.txt
  include/
    algos/
      Dijkstra.hpp
      Prim.hpp
    eval/
      MetricsWrapper.hpp
    graph/
      Graph.hpp
      GraphGen.hpp
    pq/
      IPriorityQueue.hpp
      FibonacciHeap.hpp
      PairingHeap.hpp
      PairingHeapAdapter.hpp
```

Notable update:

- Previous `src/`-based paths are no longer part of the active layout.
- Heap implementations are header-only in `include/pq/`.

## What the Benchmark Does

`main.cpp` runs all combinations of:

- 3 graph families
- 2 priority queues
- 2 algorithms

Total cases per run: `3 * 2 * 2 = 12`.

For each case, the program logs:

- Graph / heap / algorithm metadata
- Algorithm result summary
- End-to-end wall time (`WallTimeNs`)
- Priority queue operation counts:
  - `InsertCount`
  - `ExtractMinCount`
  - `DecreaseKeyCount`
- Priority queue operation timing totals:
  - `InsertTimeNs`
  - `ExtractMinTimeNs`
  - `DecreaseKeyTimeNs`

A ranked summary is produced for each graph family at the end of `results.txt`.

## Benchmark Configuration (from `main.cpp`)

Graph cases currently used:

- `Sparse`: `n=200`, `m=300`, weights `[1, 50]`, seed `1234`
- `Dense`: `n=200`, `m=5000`, weights `[1, 50]`, seed `5678`
- `HeavilyUnordered`: `n=200`, `m=1200`, weights `[1, 5000]`, seed `9012`

Graph generation currently uses:

- `makeConnectedRandomUndirected(...)` from `include/graph/GraphGen.hpp`

## Architecture Overview

### 1) Algorithm Abstraction

Both algorithms consume the same interface:

- `IPriorityQueue<KeyT>` in `include/pq/IPriorityQueue.hpp`

This allows heap implementations to be swapped without changing algorithm code.

### 2) Priority Queue Implementations

- `FibonacciHeap<KeyT>` in `include/pq/FibonacciHeap.hpp`
- `PairingHeap<T, Compare>` in `include/pq/PairingHeap.hpp`
- `PairingHeapAdapter<KeyT>` in `include/pq/PairingHeapAdapter.hpp` bridges PairingHeap to `IPriorityQueue`

### 3) Metrics Collection

`MetricsWrapper<KeyT, InnerPQ>` in `include/eval/MetricsWrapper.hpp` wraps a queue implementation and measures:

- per-operation call counts
- per-operation cumulative nanoseconds

This keeps instrumentation separate from heap logic.

### 4) Graph Model

`Graph` in `include/graph/Graph.hpp` is an adjacency-list weighted graph supporting directed or undirected edges.

Generators in `include/graph/GraphGen.hpp` include:

- connected random undirected graphs
- 2D grid graphs
- decrease-key stress graphs

## Build and Run

## Important include-path note

`main.cpp` currently includes headers using paths like:

```cpp
#include "../include/algos/Dijkstra.hpp"
```

Because of that, compile from a subdirectory where `../include` resolves correctly (example: `build/` under repo root), or adjust include directives before compiling from root.

### Option A: Build from a `build` directory (no source edits)

```powershell
mkdir build
cd build
g++ -std=c++17 -O2 -I../include ../main.cpp -o pqbench
./pqbench
```

### Option B: Build with Clang from `build`

```powershell
mkdir build
cd build
clang++ -std=c++17 -O2 -I../include ../main.cpp -o pqbench
./pqbench
```

Expected output message:

```text
Results written to results.txt
```

When running from `build`, output lands in `build/results.txt`.

## Output Format

`results.txt` is organized as:

1. Header
2. Repeated `Test Case` blocks
3. `Summary` section with per-graph rankings

Dijkstra case output includes:

- `Type: Dijkstra`
- `DistCount`
- `DistSample` (first 8 values max)

Prim case output includes:

- `Type: Prim`
- `TotalWeight`

## Reproducibility and Fairness

Inputs are deterministic due to fixed seeds.
For each graph family, one graph instance is generated and reused across all four heap/algorithm combinations for that family.
This improves fairness of direct comparisons.

## Extending the Project

### Add a new heap

1. Implement `IPriorityQueue<KeyT>`.
2. Return stable handles from `insert`.
3. Support `decreaseKey(handle, newKey)` correctly.
4. Add it to the benchmark loop in `main.cpp`.
5. Wrap it with `MetricsWrapper` for comparable metrics.

### Add a new graph scenario

1. Add a new `GraphCase` in `main.cpp`.
2. Update `buildGraph(...)` if generation strategy differs.
3. Re-run and compare summary rankings.

### Add a new algorithm

1. Implement the algorithm against `IPriorityQueue<KeyT>`.
2. Integrate algorithm selection in `main.cpp`.
3. Add result logging fields and summary ranking entry.

## Known Caveats

- The current include style in `main.cpp` assumes `../include/...` resolution.
- `results.txt` currently tracks timing and operation counts, but not memory usage.
- Benchmarks are sensitive to machine load, compiler choice, and optimization flags; compare trends across multiple runs.

## Pipeline Diagram

See `CodePipeline.txt` for the process flow from graph generation through metrics and summary output.
