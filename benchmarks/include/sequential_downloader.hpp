#ifndef SEQUENTIAL_DOWNLOADER_HPP
#define SEQUENTIAL_DOWNLOADER_HPP

#include <string>
#include <vector>
#include <cstddef>

// Write callback used by libcurl
size_t sequential_write_data(void* ptr, size_t size, size_t nmemb, void* userdata);

// Extract filename from URL
std::string sequential_extract_filename(const std::string& url);

// Download a single URL to a given output path
bool sequential_download_url(const std::string& url, const std::string& output_path, std::atomic<std::size_t>& total_sequential_bytes_downloaded);

// Read all URLs from a given text file
std::vector<std::string> sequential_read_urls(const std::string& filename);

// Perform the full sequential download of all URLs in the file
double sequential_downloader(std::atomic<std::size_t>& total_sequential_bytes_downloaded);

#endif // SEQUENTIAL_DOWNLOADER_HPP
