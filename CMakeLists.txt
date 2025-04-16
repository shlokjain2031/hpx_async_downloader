cmake_minimum_required(VERSION 3.30)
project(hpx_async_downloader)
find_package(Boost REQUIRED)
find_package(HPX REQUIRED)
FIND_PACKAGE(CURL REQUIRED)
include_directories(include)

set(CMAKE_CXX_STANDARD 20)

add_executable(hpx_async_downloader main.cpp
        include/core/scheduler.hpp
        include/core/dispatcher.hpp
        include/input/url_reader.hpp
        include/io/downloader.hpp
        include/utils/http.hpp
        include/core/shared_queue.hpp
        src/core/url_reader.cpp
)

target_link_libraries(hpx_async_downloader
        HPX::hpx
        HPX::wrap_main
        HPX::iostreams_component
#        Boost::system
#        Boost::thread
#        Boost::regex
        CURL::libcurl
        pthread
        /opt/homebrew/opt/hwloc/lib/libhwloc.15.dylib
        "-framework CoreFoundation"
        "-framework IOKit"
)
