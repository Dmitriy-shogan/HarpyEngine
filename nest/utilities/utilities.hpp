#pragma once
#ifndef HARPY_UTILS
#define HARPY_UTILS
#include "initializations.hpp"

namespace harpy_nest {
    
class utilities
{
    utilities() = default;
public:


    static std::vector<char> read_file(std::string filepath)
    {
        std::ifstream file(filepath, std::ios::ate | std::ios::binary);
        std::vector<char> res(file.tellg());
        file.seekg(0);
        file.read(res.data(), res.size());
        file.close();
        return res;
    };
};
    
}

#endif //HARPY_UTILS
