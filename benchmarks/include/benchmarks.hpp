//
// Created by Shlok Jain on 18/04/25.
//

#pragma once

#ifndef BENCHMARKS_HPP
#define BENCHMARKS_HPP

#include <cstddef>

double calculate_speedup(double sequential_time, double parallel_time);
double calculate_efficiency(double speedup, unsigned int num_threads);
double calculate_throughput(size_t tasks_completed, double total_time_seconds);
double calculate_overhead(double sequential_time, double parallel_time, double speedup);

#endif //BENCHMARKS_HPP
