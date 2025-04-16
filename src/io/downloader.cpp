//
// Created by Shlok Jain on 16/04/25.
//

#include <hpx/futures/future.hpp>
#include </Users/shlokjain/CLionProjects/hpx_async_downloader/include/core/shared_queue.hpp>
#include </Users/shlokjain/CLionProjects/hpx_async_downloader/include/utils/file_handling.hpp>
#include </Users/shlokjain/CLionProjects/hpx_async_downloader/include/utils/http.hpp>
#include <hpx/future.hpp>

void process_downloads(SharedQueue<std::string>& queue) {
    std::cout << "Processing downloads: " << queue.is_empty() << std::endl;
    while (!queue.is_empty()) {
        std::string url;
        if (queue.pop(url)) {
            try {
                std::string output_path = generate_output_path(url);
                fetch_download_request(url, output_path);
            } catch (const std::exception& e) {
                std::cerr << "Error processing URL " << url << ": " << e.what() << '\n';
            }
        } else {
            // Yield if queue is temporarily empty
            hpx::this_thread::yield();
        }
    }
}

hpx::future<void> downloader_worker(SharedQueue<std::string>& queue) {
    std::cout << "Processing downloads woker: " << queue.is_empty() << std::endl;
    return hpx::async(&process_downloads, std::ref(queue));
}

