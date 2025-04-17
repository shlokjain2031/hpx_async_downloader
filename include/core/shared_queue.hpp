//
// Created by Shlok Jain on 16/04/25.
//
#pragma once

#ifndef SHARED_QUEUE_HPP
#define SHARED_QUEUE_HPP
#include <string>
#include <queue>
#include <hpx/mutex.hpp>
#include <iostream>

// Thread-Safe Queue
template <typename T>
class SharedUrlQueue {
public:
    // Push a URL into the queue
    void push(T& url) {
        try {
            std::unique_lock<std::mutex> lock(mutex_);
            queue_.push(url);
            cv_.notify_one();
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

    // Pop a URL from the queue
    T pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        // wait until queue is not empty
        cv_.wait(lock,
                    [this]() { return !queue_.empty() || done_ ;});

        // retrieve item
        T url = queue_.front();
        queue_.pop();

        // return item
        return url;
    }

    // Check if the queue is empty
    bool is_empty() {
        std::unique_lock<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    // Set the queue to "done" state
    void set_done() {
        std::unique_lock<std::mutex> lock(mutex_);
        done_ = true;
        cv_.notify_all();  // Notify all threads that we're done
    }

    // Check if the queue is done
    bool is_done() const {
        return done_;
    }

private:
    std::queue<T> queue_;  // Internal queue
    std::mutex mutex_;       // Mutex for thread safety
    std::condition_variable cv_;     // Condition variable for efficient waiting
    std::atomic<bool> done_{false}; // Flag to signal when we're done adding data

};
#endif //SHARED_QUEUE_HPP