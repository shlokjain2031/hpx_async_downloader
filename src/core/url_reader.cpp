//
// Created by Shlok Jain on 16/04/25.
//

#include </Users/shlokjain/CLionProjects/hpx_async_downloader/include/core/shared_queue.hpp>
#include <hpx/include/async.hpp>
#include <fstream>
#include <string>
#include </Users/shlokjain/CLionProjects/hpx_async_downloader/include/input/url_reader.hpp>

void read_urls_to_queue(const std::string& filename, SharedQueue& queue) {
    std::ifstream file(filename);
    std::string url;

    // Read each line (URL) from the file
    while (std::getline(file, url)) {
        queue.push(url);  // Push URL into the shared queue
    }

    queue.set_done();  // Signal that reading is done
}