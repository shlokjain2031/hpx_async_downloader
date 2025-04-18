//
// Created by Shlok Jain on 16/04/25.
//

#ifndef HTTP_HPP
#define HTTP_HPP

bool fetch_download_request(const std::string& url, const std::string& output_path, std::atomic<std::size_t>& total_parallel_bytes_downloaded);
#endif //HTTP_HPP
