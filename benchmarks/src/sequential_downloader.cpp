#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <curl/curl.h>
#include <cstdlib> // for system()

#define URL_PATH "/Users/shlokjain/CLionProjects/hpx_async_downloader/urls.txt"
#define BENCHMARKS_PATH "/Users/shlokjain/CLionProjects/hpx_async_downloader/benchmarks/"

// Write callback for curl
size_t sequential_write_data(void* ptr, size_t size, size_t nmemb, void* userdata) {
    std::ofstream* stream = static_cast<std::ofstream*>(userdata);
    stream->write(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}

// Extracts filename from URL
std::string sequential_extract_filename(const std::string& url) {
    auto pos = url.find_last_of('/');
    return (pos != std::string::npos) ? url.substr(pos + 1) : "downloaded_file";
}

// Download a single URL
bool sequential_download_url(const std::string& url, const std::string& output_path, std::atomic<std::size_t>& total_sequential_bytes_downloaded) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    std::ofstream file(output_path, std::ios::binary);
    if (!file.is_open()) return false;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, sequential_write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);

    curl_off_t downloaded_bytes = 0;
    curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD_T, &downloaded_bytes);
    total_sequential_bytes_downloaded += downloaded_bytes;

    curl_easy_cleanup(curl);
    file.close();

    return res == CURLE_OK;
}

// Read URLs from file
std::vector<std::string> sequential_read_urls(const std::string& filename) {
    std::vector<std::string> urls;
    std::ifstream infile(filename);
    std::string url;
    while (std::getline(infile, url)) {
        if (!url.empty()) {
            urls.push_back(url);
        }
    }
    return urls;
}

double sequential_downloader(std::atomic<std::size_t>& total_sequential_bytes_downloaded) {
    // Ensure downloads/ directory exists (portable)
    system("mkdir -p downloads");

    std::string url_file = URL_PATH;
    const auto urls = sequential_read_urls(url_file);

    const auto start = std::chrono::high_resolution_clock::now();

    for (const auto& url : urls) {
        std::string filename = sequential_extract_filename(url);
        std::string output_path = BENCHMARKS_PATH + filename;

        if (sequential_download_url(url, output_path, total_sequential_bytes_downloaded)) {
            std::cout << "Downloaded Seq: " << url << " -> " << output_path << "\n";
        } else {
            std::cerr << "Failed to download: " << url << "\n";
        }
    }

    const auto end = std::chrono::high_resolution_clock::now();
    const auto elapsed = std::chrono::duration<double>(end - start).count();


    return elapsed;

}
