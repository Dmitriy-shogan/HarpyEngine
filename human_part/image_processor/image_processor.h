#pragma once
#ifndef HARPY_IMAGE_PROCESSOR
#define HARPY_IMAGE_PROCESSOR
#include <utilities/harpy_little_error.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

namespace harpy::textures
{
    //For later improving
    class image_processor
    {
        std::string last_file;
    public:

        cv::Mat read_image(std::string filepath)
        {
            last_file = filepath;
            return cv::imread(filepath);
        }

        std::vector<char> get_raw_pixels(cv::Mat image)
        {
            return std::vector<char>{image.data, image.channels() * image.cols * image.rows + image.data}; 
        }
    
    };
}

#endif //HARPY_IMAGE_PROCESSOR