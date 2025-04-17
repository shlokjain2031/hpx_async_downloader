//
// Created by Shlok Jain on 16/04/25.
//

#include <hpx/futures/future.hpp>
#include </Users/shlokjain/CLionProjects/hpx_async_downloader/include/core/shared_queue.hpp>
#include </Users/shlokjain/CLionProjects/hpx_async_downloader/include/utils/file_handling.hpp>
#include </Users/shlokjain/CLionProjects/hpx_async_downloader/include/utils/http.hpp>
#include <hpx/future.hpp>

void process_downloads(SharedUrlQueue<std::string>& queue) {
    while (!queue.is_done() || !queue.is_empty()) {
        std::string url = queue.pop();
        if (!url.empty()) {
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

hpx::future<void> downloader_worker(SharedUrlQueue<std::string>& queue) {
    return hpx::async(&process_downloads, std::ref(queue));
}

