#pragma once

#ifndef HARPY_SOUND
#define HARPY_SOUND
#pragma comment(lib, "avcodec.lib")
#include <nest/utilities/harpy_little_error.h>
#include <libavcodec/avcodec.h>
#include <AL/efx.h>

namespace harpy_sound{
class base_sound
{
    std::unique_ptr<char> data{std::make_unique<char>()};

public:
    base_sound(std::string way_to_file)
    {
        read(way_to_file);
        
    }

    void read(std::string way_to_file)
    {
        std::ifstream reader(way_to_file);
        reader.seekg(0, std::ios::end);
        int file_size = reader.tellg();
        reader.seekg(0, std::ios::beg);
        reader.read(data.get(), file_size);
    }
    
};
}


#endif //HARPY_SOUND