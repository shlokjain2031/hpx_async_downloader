//
// Created by Shlok Jain on 16/04/25.
//

#ifndef DOWNLOADER_HPP
#define DOWNLOADER_HPP
#include <hpx/futures/future.hpp>
#include </Users/shlokjain/CLionProjects/hpx_async_downloader/include/core/shared_queue.hpp>


void process_downloads(SharedUrlQueue<std::string>& queue);

hpx::future<void> downloader_worker(SharedUrlQueue<std::string>& queue);
#endif //DOWNLOADER_HPP