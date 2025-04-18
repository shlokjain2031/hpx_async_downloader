//
// Created by Shlok Jain on 16/04/25.
//

#include <hpx/futures/future.hpp>
#include </Users/shlokjain/CLionProjects/hpx_async_downloader/include/core/shared_queue.hpp>
#include </Users/shlokjain/CLionProjects/hpx_async_downloader/include/utils/file_handling.hpp>
#include </Users/shlokjain/CLionProjects/hpx_async_downloader/include/utils/http.hpp>
#include <curl/system.h>
#include <hpx/future.hpp>

void process_downloads(SharedUrlQueue<std::string>& queue, std::atomic<std::size_t>& total_parallel_bytes_downloaded) {
    while (!queue.is_done() || !queue.is_empty()) {
        std::string url = queue.pop();
        if (!url.empty()) {
            try {
                std::string output_path = generate_output_path(url);
                curl_off_t downloaded_bytes = fetch_download_request(url, output_path);
                if (downloaded_bytes != 0) {
                    total_parallel_bytes_downloaded += downloaded_bytes;
                }
                else {
                    // Log
                }
            } catch (const std::exception& e) {
                std::cerr << "Error processing URL " << url << ": " << e.what() << '\n';
            }
        } else {
            // Yield if queue is temporarily empty
            hpx::this_thread::yield();
        }
    }
}

hpx::future<void> downloader_worker(SharedUrlQueue<std::string>& queue, std::atomic<std::size_t>& total_parallel_bytes_downloaded) {
    return hpx::async(&process_downloads, std::ref(queue), std::ref(total_parallel_bytes_downloaded));
}

