#pragma once
#ifndef HARPY_AUDIO_WAV
#define HARPY_AUDIO_WAV

#include <fstream>

class wav
{
    int file_size{};
    int channels{};
    int sample_rate{};
    int depth{};

    std::unique_ptr<char> data = std::make_unique<char>();
public:

    void read_wav(std::string way_to_file)
    {
        using namespace std;
        
        ifstream reader(way_to_file, std::ios::binary);
        if (!reader.is_open())
            throw std::runtime_error("Can't open your wav file");

        reader.seekg(0, std::ios::end);
        file_size = static_cast<int>(reader.tellg());
        reader.seekg(22, std::ios::beg);

        char chans[2];
        reader.read(chans, 2);
        channels = *reinterpret_cast<int*>(chans);
        
        char samplr[4];
        reader.read(samplr, 4);
        reader.seekg(6);

        char dep[2];
        reader.read(dep, 2);

        reader.seekg(44, std::ios::beg);
        reader.read(data.get(), file_size-44);
    }
    
};

#endif