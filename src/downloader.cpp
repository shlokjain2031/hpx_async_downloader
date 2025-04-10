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

int total_urls = 0;

// Read URLs and push to queue
void process_urls(const std::string& filename, SafeQueue<std::string>& queue, std::atomic<bool>& done) {
    std::ifstream file(filename);
    std::string url;
    while (std::getline(file, url)) {
        // std::cout << "Worker on push[" << std::this_thread::get_id() << "] started.\n";
        queue.push(url);
    }
    done = true;

}

// Worker thread: send request and start download
void worker(SafeQueue<std::string>& queue, std::atomic<bool>& done,
    std::vector<std::future<void>>& futures, std::mutex& futures_mutex) {

    while (!done || !queue.empty()) {
        std::string url;
        if (queue.pop(url)) {
            // Generate output path
            std::string output_path = generate_output_path(url);

            // Launch download task
            std::future<void> future = process_download_request(url, output_path);

            // Store the future to wait later
            std::lock_guard<std::mutex> lock(futures_mutex);
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

std::future<void> process_download_request(const std::string& url, const std::string& output_path) {
    // Create a promise and future for tracking
    std::promise<void> completion_promise;
    std::future<void> future = completion_promise.get_future();

    std::thread([url, output_path, completion_promise = std::move(completion_promise)]() mutable {
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
            // Progress callback
            curl_easy_setopt(handle, CURLOPT_XFERINFOFUNCTION, +[](void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t /*ultotal*/, curl_off_t /*ulnow*/) -> int {
                if (dltotal > 0) {
                    int percentage = static_cast<int>((dlnow * 100) / dltotal);
                    std::string* url_ptr = static_cast<std::string*>(clientp);

                    std::lock_guard<std::mutex> lock(cout_mutex);

                    std::cout << "\rDownloading: [" << *url_ptr << "]: " << percentage << "% " << std::flush;
                }
                return 0;
            });
            curl_easy_setopt(handle, CURLOPT_XFERINFODATA, static_cast<void*>(const_cast<std::string*>(&url)));
            curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 0L); // Enable progress meter

            CURLcode res = curl_easy_perform(handle);

            if (res != CURLE_OK) {
                std::cerr << "\nDownload error: " << curl_easy_strerror(res) << std::endl;
            }

            outfile.close();
            curl_easy_cleanup(handle);
        }

        curl_global_cleanup();

        ++completed_downloads;

        // Notify completion
        completion_promise.set_value();

    }).detach(); // Detached thread, as we are using future/promise for sync

    return future;
}

void show_progress_bar(std::atomic<int>& completed_downloads, int total_urls) {
    const int bar_width = 50;
    auto start_time = std::chrono::steady_clock::now();

    while (completed_downloads.load() < total_urls) {
        int finished = completed_downloads.load();
        float progress = static_cast<float>(finished) / total_urls;

        auto now = std::chrono::steady_clock::now();
        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count();

        // Estimate ETA
        int estimated_total_time = (progress > 0.0) ? int(elapsed_seconds / progress) : 0;
        int eta_seconds = estimated_total_time - elapsed_seconds;

        std::lock_guard<std::mutex> lock(cout_mutex);
        // Build progress bar string
        std::cout << "\r[";
        int pos = static_cast<int>(bar_width * progress);
        for (int i = 0; i < bar_width; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }

        std::cout << "] " << std::setw(3) << int(progress * 100.0) << "% ("
                  << finished << "/" << total_urls << ") "
                  << "Elapsed: " << elapsed_seconds << "s "
                  << "ETA: " << (eta_seconds > 0 ? eta_seconds : 0) << "s"
                  << std::flush;

        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Refresh rate
    }

    // Final full bar
    std::cout << "\r[";
    for (int i = 0; i < bar_width; ++i) std::cout << "=";
    std::cout << "] 100% (" << total_urls << "/" << total_urls << ") Completed!"
              << std::endl;
}


