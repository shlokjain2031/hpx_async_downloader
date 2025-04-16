//
// Created by Shlok Jain on 16/04/25.
//

#ifndef URL_READER_HPP
#define URL_READER_HPP

#include <core/shared_queue.hpp>
#include <hpx/include/async.hpp>
#include <fstream>
#include <string>

#endif //URL_READER_HPP

void read_urls_to_queue(const std::string& filename, SharedQueue& queue);

