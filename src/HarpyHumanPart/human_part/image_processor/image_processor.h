#pragma once
#ifndef HARPY_IMAGE_PROCESSOR
#define HARPY_IMAGE_PROCESSOR
#include <utilities/harpy_little_error.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

namespace harpy
{
    //For later improving
    class image_processor
    {
    public:

        static cv::Mat read_image(std::string filepath)
        {
            return cv::imread(filepath);
        }

        static std::vector<unsigned char> get_raw_pixels(cv::Mat image)
        {
            return std::vector<unsigned char>{image.data, image.channels() * image.cols * image.rows + image.data}; 
        }
    
    };
}

#endif //HARPY_IMAGE_PROCESSOR