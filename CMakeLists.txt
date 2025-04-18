cmake_minimum_required(VERSION 3.30)
project(hpx_async_downloader)
find_package(Boost REQUIRED)
find_package(HPX REQUIRED)
FIND_PACKAGE(CURL REQUIRED)
include_directories(include)

set(CMAKE_CXX_STANDARD 20)

add_executable(hpx_async_downloader main.cpp
        include/input/url_reader.hpp
        include/io/downloader.hpp
        include/utils/http.hpp
        include/core/shared_queue.hpp
        src/input/url_reader.cpp
        src/utils/http.cpp
        src/io/downloader.cpp
        include/utils/file_handling.hpp
        src/utils/file_handling.cpp
        include/input/url_reader_promise.hpp
        benchmarks/src/sequential_downloader.cpp
        benchmarks/include/sequential_downloader.hpp
        benchmarks/include/benchmarks.hpp
        benchmarks/src/benchmarks.cpp
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
