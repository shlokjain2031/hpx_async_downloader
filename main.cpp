#include <iostream>
#include <hpx/init.hpp>

#include <hpx/future.hpp>
#include </Users/shlokjain/CLionProjects/hpx_async_downloader/include/core/shared_queue.hpp>
#include </Users/shlokjain/CLionProjects/hpx_async_downloader/include/input/url_reader.hpp>
#include </Users/shlokjain/CLionProjects/hpx_async_downloader/include/io/downloader.hpp>

#define URL_PATH "/Users/shlokjain/CLionProjects/hpx_async_downloader/urls.txt"

int my_hpx_main(hpx::program_options::variables_map& vm) {
    SharedUrlQueue<std::string> queue;
    std::vector<hpx::future<void>> futures;
    std::mutex futures_mutex;

    hpx::future<void> producer_future = hpx::async(&read_urls_to_queue, URL_PATH, std::ref(queue));

    const unsigned int num_cores = std::thread::hardware_concurrency();
    const unsigned int num_threads = (num_cores > 2) ? num_cores - 2 : 1;  // Ensure at least one thread is used

    std::cout << "Using " << num_threads << " downloader threads out of " << num_cores << " cores." << std::endl;
    for (unsigned int i = 0; i < num_threads; i++) {
        hpx::future<void> downloader_future = hpx::async(&downloader_worker, std::ref(queue));

        // Store the future to wait later
        std::lock_guard<std::mutex> lock(futures_mutex);
        futures.push_back(std::move(downloader_future));
    }

    producer_future.get();
    queue.set_done();

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
