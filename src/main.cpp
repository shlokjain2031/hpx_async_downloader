#include <iostream>
#include <hpx/init.hpp>

#include <downloader.hpp>

#define URL_PATH "/Users/shlokjain/CLionProjects/hpx_async_downloader/urls.txt"

int my_hpx_main(hpx::program_options::variables_map& vm) {
    // All your async code will go here

    SafeQueue<std::string> queue;
    std::atomic<bool> done = false;
    std::vector<std::future<void>> futures;

    // Producer thread: reads URLs and fills the queue
    std::thread producer(processUrls, URL_PATH, std::ref(queue), std::ref(done));

    // Launch multiple worker threads
    const int num_workers = std::thread::hardware_concurrency();
    std::vector<std::thread> workers;
    for (int i = 0; i < num_workers; ++i) {
        workers.emplace_back([&queue, &done, &futures]() {
                worker(queue, done, futures);
            });
    }

    // Wait for producer to finish
    producer.join();

    // Wait for workers to finish
    for (auto& w : workers) {
        w.join();
    }

    // Wait for all downloads to complete
    for (auto& f : futures) {
        f.wait();
    }

    std::cout << "All downloads completed!" << std::endl;

    return hpx::local::finalize();
}

int main(int argc, char* argv[]) {
    return hpx::local::init(my_hpx_main, argc, argv);
}
