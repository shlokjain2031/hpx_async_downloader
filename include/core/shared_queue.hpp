//
// Created by Shlok Jain on 16/04/25.
//

#ifndef SHARED_QUEUE_HPP
#define SHARED_QUEUE_HPP
#include <hpx/include/async.hpp>
#include <string>
#include <atomic>
#include <queue>

class SharedQueue {
public:
    // Push a URL into the queue
    void push(const std::string& url) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(url);
        cv_.notify_one();  // Notify a waiting thread that a new item is available
    }

    // Pop a URL from the queue
    bool pop(std::string& url) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return false;
        }
        url = queue_.front();
        queue_.pop();
        return true;
    }

    // Wait for and pop a URL from the queue (blocking)
    void wait_and_pop(std::string& url) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]() { return !queue_.empty() || done_; }); // Wait until there's data or we're done

        if (!queue_.empty()) {
            url = queue_.front();
            queue_.pop();
        }
    }

    // Set the queue to "done" state
    void set_done() {
        std::lock_guard<std::mutex> lock(mutex_);
        done_ = true;
        cv_.notify_all();  // Notify all threads that we're done
    }

    // Check if the queue is done
    bool is_done() const {
        return done_;
    }

private:
    std::queue<std::string> queue_;  // Internal queue
    mutable std::mutex mutex_;       // Mutex for thread safety
    std::condition_variable cv_;     // Condition variable for efficient waiting
    std::atomic<bool> done_{false}; // Flag to signal when we're done adding data

};
#endif //SHARED_QUEUE_HPP