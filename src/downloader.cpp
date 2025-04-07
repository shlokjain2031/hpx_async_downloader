// downloader.cpp
#include "/Users/shlokjain/CLionProjects/hpx_async_downloader/include/downloader.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <curl/curl.h>
#include <future>

// SafeQueue method implementations
// ... (implement push, pop, empty with locking)

// Read URLs and push to queue
void processUrls(const std::string& filename, SafeQueue<std::string>& queue, std::atomic<bool>& done) {
    std::ifstream file(filename);
    std::string url;
    while (std::getline(file, url)) {
        // std::cout << "Worker on push[" << std::this_thread::get_id() << "] started.\n";
        queue.push(url);
    }
    done = true;
}

// Worker thread: send request and start download
void worker(SafeQueue<std::string>& queue, std::atomic<bool>& done, std::vector<std::future<void>>& futures) {
    while (!done || !queue.empty()) {
        std::string url;
        if (queue.pop(url)) {
            // std::cout << "Worker on pop [" << std::this_thread::get_id() << "] started.\n";
            std::cout << "Processing URL: " << url << std::endl;
            // Generate output path
            std::string output_path = generate_output_path(url);

            // Create a promise and future for tracking
            std::promise<void> promise;
            std::future<void> future = promise.get_future();

            // Launch download task
            download_file(url, output_path, promise);

            // Store the future to wait later
            futures.push_back(std::move(future));

        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}


std::string generate_output_path(const std::string& url) {
    namespace fs = std::filesystem;

    // Get the current working directory (terminal location)
    fs::path current_path = fs::current_path();

    // Define relative download directory
    fs::path download_dir = current_path / "downloads";

    // Extract filename from URL
    auto last_slash_pos = url.find_last_of('/');
    if (last_slash_pos == std::string::npos || last_slash_pos + 1 >= url.size()) {
        throw std::runtime_error("Invalid URL format, cannot extract filename: " + url);
    }

    std::string filename = url.substr(last_slash_pos + 1);

    // Ensure download directory exists
    if (!fs::exists(download_dir)) {
        fs::create_directories(download_dir);
    }

    // Full output path
    fs::path output_path = download_dir / filename;

    return output_path.string();
}

// TODO: Set the output_path when the function is called using the generate_output_path function
void download_file(const std::string& url, const std::string& output_path, std::promise<void>& completion_promise) {
    std::thread([url, output_path, &completion_promise]() mutable {
        // std::cout << "Worker Download file[" << std::this_thread::get_id() << "] started.\n";

        curl_global_init(CURL_GLOBAL_DEFAULT);
        CURL *handle = curl_easy_init();

        if (handle) {
            std::ofstream outfile(output_path, std::ios::binary);

            curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, +[](void* ptr, size_t size, size_t nmemb, void* stream) -> size_t {
                std::ofstream* out = static_cast<std::ofstream*>(stream);
                size_t written = 0;
                if (out && out->is_open()) {
                    out->write(static_cast<const char*>(ptr), size * nmemb);
                    written = size * nmemb;
                }
                return written;
            });

            curl_easy_setopt(handle, CURLOPT_WRITEDATA, &outfile);

            CURLcode res = curl_easy_perform(handle);

            if (res != CURLE_OK) {
                std::cerr << "Download error: " << curl_easy_strerror(res) << std::endl;
            } else {
                std::cout << "Downloaded: " << url << " -> " << output_path << std::endl;
            }

            outfile.close();
            curl_easy_cleanup(handle);
        }

        curl_global_cleanup();

        // Notify completion
        completion_promise.set_value();

    }).detach(); // Detached thread, as we are using future/promise for sync
}




