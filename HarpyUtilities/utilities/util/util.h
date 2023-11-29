#pragma once
#ifndef HARPY_NEST_UTILITIES_UTIL
#define HARPY_NEST_UTILITIES_UTIL
#include <string>
#include <vector>

namespace harpy::utilities
{
    std::vector<char> read_file_binary(std::string way_to_file);
    std::string read_file(std::string way_to_file);
    
    void write_file(std::string way_to_file, std::string data);
}

#endif //HARPY_NEST_UTILITIES_UTIL