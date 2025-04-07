//
// Created by Shlok Jain on 05/04/25.
//

#ifndef DOWNLOADER_HPP
#define DOWNLOADER_HPP
#include <string>
#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <future>

#endif //DOWNLOADER_HPP

// Thread-safe queue
template<typename T>
class SafeQueue {
public:
    void push(T value) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(value)); // Use move semantics for efficiency
        cond_var_.notify_one(); // Notify one waiting thread
    }

    bool pop(T& value) {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_var_.wait(lock, [this] { return !queue_.empty(); }); // Wait until queue is not empty

        if (queue_.empty()) {
            return false; // Just in case (although condition should ensure non-empty)
        }

        value = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cond_var_;
};

// Function declarations
void processUrls(const std::string& filename, SafeQueue<std::string>& queue, std::atomic<bool>& done);
void worker(SafeQueue<std::string>& queue, std::atomic<bool>& done, std::vector<std::future<void>>& futures);
void download_file(const std::string& url);
std::string generate_output_path(const std::string& url);
void download_file(const std::string& url, const std::string& output_path, std::promise<void>& completion_promise);