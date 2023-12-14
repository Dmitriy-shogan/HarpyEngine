#include "util.h"

#include <fstream>

#include <logger/harpy_little_error.h>
#include <logger/logger.h>

using logger = harpy::utilities::error_handling::logger;
using harpy_error = harpy::utilities::error_handling::harpy_little_error;

std::vector<char> harpy::utilities::read_file_binary(std::string way_to_file)
{
    std::ifstream file(way_to_file, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw error_handling::harpy_little_error("failed to open file!");
    }

    size_t file_size = file.tellg();
    std::vector<char> buffer{};
    buffer.resize(file_size);

    file.seekg(0);
    file.read(buffer.data(), static_cast<std::streamsize>(file_size));
    file.close();

    return buffer;
}

std::string harpy::utilities::read_file(std::string way_to_file)
{
    std::ifstream file(way_to_file, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw error_handling::harpy_little_error("failed to open file!");
    }
    
    std::ifstream::pos_type fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> bytes(fileSize);
    file.read(bytes.data(), fileSize);

    return std::string(bytes.data(), fileSize);
}


void harpy::utilities::write_file(std::string way_to_file, std::string data)
{
    std::ofstream file{way_to_file};
    if (!file.is_open()) {
        throw error_handling::harpy_little_error("failed to create/open file" + way_to_file);
    }
    file << data;
    file.close();
}
