#pragma once

#include "../pq/IPriorityQueue.hpp"
#include <chrono>

/*
  MetricsWrapper wraps a PQ and measures counts + time spent inside PQ operations.
  This avoids mixing timing code into heaps or algorithms.
*/
template <typename KeyT, typename InnerPQ>
class MetricsWrapper : public IPriorityQueue<KeyT>
{
public:
    using Handle = typename IPriorityQueue<KeyT>::Handle;

    explicit MetricsWrapper(InnerPQ &inner) : inner_(inner) {}

    Handle insert(const KeyT &key, int vertex) override
    {
        auto t0 = clock::now();
        Handle h = inner_.insert(key, vertex);
        auto t1 = clock::now();

        insertCount_++;
        insertTimeNs_ += elapsedNs(t0, t1);
        return h;
    }

    PQItem<KeyT> extractMin() override
    {
        auto t0 = clock::now();
        auto item = inner_.extractMin();
        auto t1 = clock::now();

        extractMinCount_++;
        extractMinTimeNs_ += elapsedNs(t0, t1);
        return item;
    }

    void decreaseKey(Handle h, const KeyT &newKey) override
    {
        auto t0 = clock::now();
        inner_.decreaseKey(h, newKey);
        auto t1 = clock::now();

        decreaseKeyCount_++;
        decreaseKeyTimeNs_ += elapsedNs(t0, t1);
    }

    bool empty() const override { return inner_.empty(); }

    long long insertCount() const { return insertCount_; }
    long long extractMinCount() const { return extractMinCount_; }
    long long decreaseKeyCount() const { return decreaseKeyCount_; }

    long long insertTimeNs() const { return insertTimeNs_; }
    long long extractMinTimeNs() const { return extractMinTimeNs_; }
    long long decreaseKeyTimeNs() const { return decreaseKeyTimeNs_; }

private:
    using clock = std::chrono::high_resolution_clock;

    InnerPQ &inner_;

    long long insertCount_ = 0;
    long long extractMinCount_ = 0;
    long long decreaseKeyCount_ = 0;

    long long insertTimeNs_ = 0;
    long long extractMinTimeNs_ = 0;
    long long decreaseKeyTimeNs_ = 0;

    static long long elapsedNs(const clock::time_point &a, const clock::time_point &b)
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(b - a).count();
    }
};
