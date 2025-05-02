//
// Created by Shlok Jain on 18/04/25.
//
#pragma once

#ifndef URL_READER_PROMISE_HPP
#define URL_READER_PROMISE_HPP

#include <coroutine>
#include <iostream>

struct ReadUrlsPromise;

struct ReadUrlsHandle : std::coroutine_handle<ReadUrlsPromise> {
    using promise_type = ReadUrlsPromise;

    ReadUrlsHandle() = default;

    // Constructor that accepts the base class
    ReadUrlsHandle(std::coroutine_handle<ReadUrlsPromise> h)
        : std::coroutine_handle<ReadUrlsPromise>(h) {}

     bool await_ready() noexcept { return false; }
     void await_resume() noexcept {}
     void await_suspend(std::coroutine_handle<ReadUrlsPromise> h) noexcept {}
};

struct ReadUrlsPromise {
    ReadUrlsHandle get_return_object() {
        return ReadUrlsHandle::from_promise(*this);
    }
     std::suspend_never initial_suspend() noexcept { return {}; }
     std::suspend_never final_suspend() noexcept { return {}; }
     void return_void() {}
     void unhandled_exception() { std::terminate(); }
};


#endif //URL_READER_PROMISE_HPP
