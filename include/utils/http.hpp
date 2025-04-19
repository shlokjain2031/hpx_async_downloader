//
// Created by Shlok Jain on 16/04/25.
//

#ifndef HTTP_HPP
#define HTTP_HPP
#include <curl/system.h>

curl_off_t fetch_download_request(const std::string& url, const std::string& output_path);
#endif //HTTP_HPP
