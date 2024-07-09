#pragma once
#ifndef HARPY_UTILITIES_IMAGES
#define HARPY_UTILITIES_IMAGES
#include <../dll_macro.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

namespace harpy::utilities::images
{
    class HARPY_UTILITIES_API image
    {
        cv::Mat cv_image{};
        
    public:
        image() = default;

        void read_image(std::string path);

        //Just for now, doing nothing
        void save_image(std::string path, std::string filename);

        const cv::Mat& get_cv_data();
        
    };
}

#endif //HARPY_UTILITIES_IMAGES