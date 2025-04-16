//
// Created by Shlok Jain on 16/04/25.
//

#include <stdexcept>
#include <string>
#include <fstream>

std::string generate_output_path(const std::string& url) {
    namespace fs = std::filesystem;

    // Get the current working directory (terminal location)
    const fs::path current_path = fs::current_path();

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
