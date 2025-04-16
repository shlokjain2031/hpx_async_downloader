#include <iostream>
#include <hpx/init.hpp>

#include <hpx/future.hpp>
#include </Users/shlokjain/CLionProjects/hpx_async_downloader/include/core/shared_queue.hpp>
#include </Users/shlokjain/CLionProjects/hpx_async_downloader/include/input/url_reader.hpp>
#include </Users/shlokjain/CLionProjects/hpx_async_downloader/include/io/downloader.hpp>

#define URL_PATH "/Users/shlokjain/CLionProjects/hpx_async_downloader/urls.txt"

int my_hpx_main(hpx::program_options::variables_map& vm) {
    SharedQueue<std::string> queue;
    hpx::async([&queue]() {
        read_urls_to_queue(URL_PATH, std::ref(queue));
    });

    const unsigned int num_cores = std::thread::hardware_concurrency();
    const unsigned int num_threads = (num_cores > 2) ? num_cores - 2 : 1;  // Ensure at least one thread is used

    std::cout << "Using " << num_threads << " downloader threads out of " << num_cores << " cores." << std::endl;
    hpx::async(&downloader_worker, std::ref(queue));

    // Allow some time for the downloads to finish (simplified for testing)
    std::this_thread::sleep_for(std::chrono::seconds());


    return hpx::local::finalize();
}

int main(int argc, char* argv[]) {
    return hpx::local::init(my_hpx_main, argc, argv);
}
