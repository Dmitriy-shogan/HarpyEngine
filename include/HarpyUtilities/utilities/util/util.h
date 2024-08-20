#pragma once
#ifndef HARPY_NEST_UTILITIES_UTIL
#define HARPY_NEST_UTILITIES_UTIL
#include <string>
#include <vector>
#include <../dll_macro.h>

namespace harpy::utilities
{
    HARPY_UTILITIES_API std::vector<char> read_file_binary(std::string way_to_file);
    HARPY_UTILITIES_API std::string read_file(std::string way_to_file);
    
    HARPY_UTILITIES_API void write_file(std::string way_to_file, std::string data);

    template<typename T>
    inline void hash_combine(std::size_t & s, const T & v)
    {
        std::hash<T> h;
        s^= h(v) + 0x9e3779b9 + (s<< 6) + (s>> 2);
    }
}

#endif //HARPY_NEST_UTILITIES_UTIL