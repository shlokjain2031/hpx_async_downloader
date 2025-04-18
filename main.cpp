#include <iostream>
#include <hpx/init.hpp>

#include <hpx/future.hpp>
#include </Users/shlokjain/CLionProjects/hpx_async_downloader/include/core/shared_queue.hpp>
#include </Users/shlokjain/CLionProjects/hpx_async_downloader/include/input/url_reader.hpp>
#include </Users/shlokjain/CLionProjects/hpx_async_downloader/include/io/downloader.hpp>

#define URL_PATH "/Users/shlokjain/CLionProjects/hpx_async_downloader/urls.txt"

int my_hpx_main(hpx::program_options::variables_map& vm) {
    SharedUrlQueue<std::string> queue;
    std::vector<hpx::future<void>> download_futures;
    std::mutex download_futures_mutex;

    const unsigned int num_cores = std::thread::hardware_concurrency();
    const unsigned int num_threads = (num_cores > 2) ? num_cores - 2 : 1;  // Ensure at least one thread is used
    std::cout << "Using " << num_threads << " downloader threads out of " << num_cores << " cores." << std::endl;

    // Start coroutine (runs until first co_await)
    constexpr std::size_t batch_size = 5;
    const auto read_urls_handle = read_urls_to_queue(URL_PATH, queue, batch_size);

    // Manually resume the coroutine until it's done
    while (!read_urls_handle.done()) {
        read_urls_handle.resume();

        for (unsigned int i = 0; i < num_threads; i++) {
            hpx::future<void> downloader_future = hpx::async(&downloader_worker, std::ref(queue));

            // Store the future to wait later
            std::lock_guard<std::mutex> lock(download_futures_mutex);
            download_futures.push_back(std::move(downloader_future));
        }
    }

    // Wait for all downloads to complete
    hpx::wait_all(download_futures);

    std::cout << "All downloads completed!" << std::endl;

    return hpx::local::finalize();
}
int main(int argc, char* argv[]) {
    return hpx::local::init(my_hpx_main, argc, argv);
}
