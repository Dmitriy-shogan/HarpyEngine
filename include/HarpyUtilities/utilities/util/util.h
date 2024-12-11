#pragma once
#ifndef HARPY_NEST_UTILITIES_UTIL
#define HARPY_NEST_UTILITIES_UTIL
#include <stringzilla/stringzilla.hpp>
#include <vector>
#include <../utilities_dll_macro.h>

namespace harpy::utilities
{
    HARPY_UTILITIES_API std::vector<char> read_file_binary(sz::string_view way_to_file);
    HARPY_UTILITIES_API sz::string read_file(sz::string_view way_to_file);
    
    HARPY_UTILITIES_API void write_file(sz::string_view way_to_file, sz::string_view data);

    template<typename T>
    inline void hash_combine(std::size_t & s, const T & v)
    {
        std::hash<T> h;
        s^= h(v) + 0x9e3779b9 + (s<< 6) + (s>> 2);
    }
}

#endif //HARPY_NEST_UTILITIES_UTIL