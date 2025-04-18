//
// Created by Shlok Jain on 16/04/25.
//

#include <curl/curl.h>
#include <string>
#include <fstream>
#include <iostream>

bool fetch_download_request(const std::string& url, const std::string& output_path, std::atomic<std::size_t>& total_parallel_bytes_downloaded) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* handle = curl_easy_init();


    if (!handle) {
        std::cerr << "Failed to initialize CURL for URL: " << url << std::endl;
        curl_global_cleanup();
        return false;
    }

    std::ofstream outfile(output_path, std::ios::binary);
    if (!outfile) {
        std::cerr << "Failed to open output file: " << output_path << std::endl;
        curl_easy_cleanup(handle);
        curl_global_cleanup();
        return false;
    }

    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION,
        +[](void* ptr, size_t size, size_t nmemb, void* stream) -> size_t {
            auto* out = static_cast<std::ofstream*>(stream);
            if (out && out->is_open()) {
                out->write(static_cast<const char*>(ptr), size * nmemb);
                return size * nmemb;
            }
            return 0;
        });

    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &outfile);
    total_parallel_bytes_downloaded += outfile.tellp();

    CURLcode res = curl_easy_perform(handle);

    outfile.close();
    curl_easy_cleanup(handle);
    curl_global_cleanup();

    if (res != CURLE_OK) {
        std::cerr << "Download error for " << url << ": " << curl_easy_strerror(res) << std::endl;
        return false;
    }

    std::cout << "Downloaded: " << url << " -> " << output_path << std::endl;
    return true;
}