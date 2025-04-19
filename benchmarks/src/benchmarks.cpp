//
// Created by Shlok Jain on 18/04/25.
//
#include <cstddef>

// Speedup = Ts / Tp
double calculate_speedup(double sequential_time, double parallel_time) {
    return sequential_time / parallel_time;
}

// Efficiency = Speedup / Number of processors (threads)
double calculate_efficiency(double speedup, unsigned int num_threads) {
    return speedup / num_threads;
}

// Throughput = Number of tasks completed / Total time taken (in seconds)
double calculate_throughput(size_t total_tasks, double total_time_seconds) {
    return total_tasks / total_time_seconds;
}
// Latency per task = Total time / Number of tasks = 1/Throughput