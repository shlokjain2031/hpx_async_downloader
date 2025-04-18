//
// Created by Shlok Jain on 16/04/25.
//

#include <fstream>
#include <string>
#include </Users/shlokjain/CLionProjects/hpx_async_downloader/include/core/shared_queue.hpp>
#include <future>
#include <iostream>
#include <coroutine>
#include <regex>

#include "input/url_reader_promise.hpp"

// Simple URL validator
bool is_valid_url(const std::string& url) {
    static const std::regex pattern(R"(https?://[^\s]+)");
    return std::regex_match(url, pattern);
}

ReadUrlsHandle read_urls_to_queue(const std::string& filename, SharedUrlQueue<std::string>& queue, std::size_t batch_size) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open URL file: " << filename << std::endl;
        co_return;
    }

    std::vector<std::string> buffer;
    std::string line;

    while (std::getline(file, line)) {
        if (is_valid_url(line)) {
            buffer.push_back(line);
        }

        if (buffer.size() == batch_size) {
            for (auto& url : buffer) {
                queue.push(url);
            }
            buffer.clear();

            co_await ReadUrlsHandle();  // Manual suspension
        }
    }

    for (auto& url : buffer) {
        queue.push(url);
    }

    queue.set_done();

    std::cout << "Finished reading URLs\n";
    co_return;
}
