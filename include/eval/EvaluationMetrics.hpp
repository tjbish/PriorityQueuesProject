#pragma once

#include <cstddef>
#include <cstdint>

/**
 * @brief Stores performance metrics for priority queue evaluation.
 *
 * Used to compare heap implementations across graph algorithms.
 */
struct EvaluationMetrics
{
    // Operation counts
    std::size_t insert_count = 0;
    std::size_t extract_min_count = 0;
    std::size_t decrease_key_count = 0;

    // Time spent in operations (milliseconds)
    double insert_time_ms = 0.0;
    double extract_min_time_ms = 0.0;
    double decrease_key_time_ms = 0.0;

    // Total runtime (milliseconds)
    double total_runtime_ms = 0.0;
};
