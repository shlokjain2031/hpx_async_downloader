#include <iostream>
#include <hpx/init.hpp>

#include "core/shared_queue.hpp"
#include "input/url_reader.hpp"


#define URL_PATH "/Users/shlokjain/CLionProjects/hpx_async_downloader/urls.txt"

int my_hpx_main(hpx::program_options::variables_map& vm) {
    SharedQueue queue;
    hpx::async(&read_urls_to_queue, URL_PATH, std::ref(queue)).get();

    // Now let's test by popping and printing URLs from the queue
    int count = 0;
    std::string url;
    while (queue.pop(url)) {
        std::cout << "Popped URL: " << url << std::endl; // Print each URL as it's popped
        count++;
    }

    // If done, we'll check if the queue is indeed empty and finished
    if (queue.is_done()) {
        std::cout << "Finished reading all URLs and queue is done!" << std::endl;
        std::cout << count << std::endl;
    }

    return hpx::local::finalize();
}

int main(int argc, char* argv[]) {
    return hpx::local::init(my_hpx_main, argc, argv);
}
