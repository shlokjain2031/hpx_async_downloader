#include <iostream>
#include <hpx/init.hpp>

#include <hpx/future.hpp>
#include </Users/shlokjain/CLionProjects/hpx_async_downloader/include/core/shared_queue.hpp>
#include </Users/shlokjain/CLionProjects/hpx_async_downloader/include/input/url_reader.hpp>
#include </Users/shlokjain/CLionProjects/hpx_async_downloader/include/io/downloader.hpp>

#include "benchmarks/include/benchmarks.hpp"
#include "benchmarks/include/sequential_downloader.hpp"

#define URL_PATH "/Users/shlokjain/CLionProjects/hpx_async_downloader/urls.txt"


double parallel_downloader(std::size_t batch_size, std::atomic<std::size_t>& total_parallel_bytes_downloaded) {

    SharedUrlQueue<std::string> queue;
    std::vector<hpx::future<void>> download_futures;
    std::mutex download_futures_mutex;

    const unsigned int num_cores = hpx::get_num_worker_threads();
    const unsigned int num_threads = (num_cores > 2) ? num_cores - 2 : 1;  // Ensure at least one thread is used
    // std::cout << "Using " << num_threads << " downloader threads out of " << num_cores << " cores." << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    // Start coroutine (runs until first co_await)
    const auto read_urls_handle = read_urls_to_queue(URL_PATH, queue, batch_size);

    // Manually resume the coroutine until it's done
    while (!read_urls_handle.done()) {
        read_urls_handle.resume();

        for (unsigned int i = 0; i < num_threads; i++) {
            hpx::future<void> downloader_future = hpx::async(&downloader_worker, std::ref(queue), std::ref(total_parallel_bytes_downloaded));

            // Store the future to wait later
            std::lock_guard<std::mutex> lock(download_futures_mutex);
            download_futures.push_back(std::move(downloader_future));
        }
    }

    // Wait for all downloads to complete
    hpx::wait_all(download_futures);

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration<double>(end - start).count();

    return elapsed;
}

void print_pretty(std::size_t batch_size, std::size_t num_of_urls,  std::size_t total_parallel_bytes_downloaded,
                    unsigned int num_threads, double time_taken, double speedup, double efficiency,
                    double throughput, double latency_per_task) {
    std::cout << "\nTime taken: " << time_taken << " sec" << std::endl;
    std::cout << "Batch Size: " << batch_size << std::endl;
    std::cout << "Number of URLs: " << num_of_urls << std::endl;
    std::cout << "Total Bytes Downloaded: " << total_parallel_bytes_downloaded << std::endl;
    std::cout << "Speedup: " << speedup << std::endl;
    std::cout << "Number of threads: " << num_threads << std::endl;
    std::cout << "Efficiency: " << efficiency * 100 << "%" << std::endl;
    std::cout << "Throughput: " << throughput << std::endl;
    std::cout << "Latency per task: " << latency_per_task << std::endl;
}

void print_benchmarks_csv(std::size_t batch_size, std::size_t num_of_urls, std::size_t total_parallel_bytes_downloaded, int num_threads, double time_taken, double speedup,
                            double efficiency, double throughput_parallel, double latency_per_task_parallel,
                            double throughput_sequential, double latency_per_task_sequential) {
    system("clear");
    std::cout << batch_size << ","
              << num_of_urls << ','
              << total_parallel_bytes_downloaded << ','
              << num_threads << ","
              << time_taken << ","
              << speedup << ","
              << efficiency << ","
              << throughput_parallel << ","
              << latency_per_task_parallel << ","
              << throughput_sequential << ","
              << latency_per_task_sequential << std::endl;
}


int my_hpx_main(const hpx::program_options::variables_map& vm) {
    // benchmarking, batch size from compiler options
    const int benchmarking = vm["benchmarking"].as<int>();
    const std::size_t batch_size = vm["batch_size"].as<std::size_t>();
    const std::size_t num_of_urls = vm["num_of_urls"].as<std::size_t>();
    assert(num_of_urls>=batch_size);

    std::atomic<std::size_t> total_parallel_bytes_downloaded = 0;
    std::atomic<std::size_t> total_sequential_bytes_downloaded = 0;

    // std::cout << "\nRunning Parallel Async File Downloader\n";
    const double parallel_elapsed = parallel_downloader(batch_size, total_parallel_bytes_downloaded);

    // std::cout << "\nRunning Sequential Async File Downloader\n";
    const double sequential_elapsed = sequential_downloader(total_sequential_bytes_downloaded);

    const double speedup = calculate_speedup(sequential_elapsed, parallel_elapsed);
    const std::size_t num_of_threads = hpx::get_num_worker_threads();
    const double efficiency = calculate_efficiency(speedup, num_of_threads);
    const double throughput_parallel = calculate_throughput(total_parallel_bytes_downloaded.load(), parallel_elapsed);
    const double throughput_sequential = calculate_throughput(total_sequential_bytes_downloaded.load(), parallel_elapsed);
    const double latency_per_task_parallel = 1/throughput_parallel;
    const double latency_per_task_sequential = 1/throughput_sequential;

    if (benchmarking) {
        print_benchmarks_csv(batch_size, num_of_urls, total_parallel_bytes_downloaded.load(), num_of_threads,
            parallel_elapsed, speedup, efficiency, throughput_parallel,
            latency_per_task_parallel, throughput_sequential, latency_per_task_sequential);
    }
    else {
        print_pretty(batch_size, num_of_urls, total_parallel_bytes_downloaded.load(), num_of_threads, parallel_elapsed,
                        speedup, efficiency, throughput_parallel, latency_per_task_parallel);
    }
    return hpx::local::finalize();
}
int main(int argc, char* argv[]) {
    using namespace hpx::program_options;
    options_description cmdline("usage: " HPX_APPLICATION_STRING " [options]");
    // clang-format off
    cmdline.add_options()
            ("benchmarking", hpx::program_options::value<int>()->default_value(1), "Benchmarking?")
            ("batch_size", hpx::program_options::value<std::size_t>()->default_value(1), "Batch size")
            ("num_of_urls", hpx::program_options::value<std::size_t>()->default_value(3), "Number of URLs");

    // clang-format on
    hpx::local::init_params init_args;
    init_args.desc_cmdline = cmdline;

    return hpx::local::init(my_hpx_main, argc, argv, init_args);
}
