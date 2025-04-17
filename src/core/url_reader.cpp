//
// Created by Shlok Jain on 16/04/25.
//

#include <fstream>
#include <string>
#include </Users/shlokjain/CLionProjects/hpx_async_downloader/include/core/shared_queue.hpp>
#include <iostream>

void read_urls_to_queue(const std::string& filename, SharedUrlQueue<std::string>& queue) {
    std::ifstream file(filename);
    std::string url;
    int count = 0;

    // Read each line (URL) from the file
    while (std::getline(file, url)) {
        queue.push(url);  // Push URL into the shared queue
        count++;
    }

    std::cout << "Total URLs: " << count << std::endl;
    queue.set_done();  // Signal that reading is done
}
