//
// Created by Shlok Jain on 16/04/25.
//

#ifndef URL_READER_HPP
#define URL_READER_HPP

#include </Users/shlokjain/CLionProjects/hpx_async_downloader/include/core/shared_queue.hpp>
#include <string>

#include "url_reader_promise.hpp"

bool is_valid_url(const std::string& url);

ReadUrlsHandle read_urls_to_queue(const std::string& filename, SharedUrlQueue<std::string>& queue);

#endif //URL_READER_HPP