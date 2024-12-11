#pragma once
#ifndef HARPY_UTILITIES_IMAGES
#define HARPY_UTILITIES_IMAGES
#include <../utilities_dll_macro.h>
#include <stringzilla/stringzilla.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

namespace harpy::utilities
{
    enum image_format{
        A = 1 << 0,
        B = 1 << 1,
        G = 1 << 2,
        R = 1 << 3,
        is_BGR = 1 << 4,
        is_GRAY = 1 << 5,
        RGB = R|G|B,
        BGR = RGB|is_BGR,
        RGBA = RGB|A,
        BGRA = BGR|A,

        he_first = A,
        he_bm = 1,
        he_sum = 10
    };

    class HARPY_UTILITIES_API image
    {
        cv::Mat cv_image{};
        //Only because of int don't want to be an image format
        int current_format{};
        
    public:
        image() = default;
        image(sz::string_view path);

        void read_image(sz::string_view path);

        //NOT IMPLEMENTED
        void save_image(sz::string_view path, sz::string_view filename);

        std::pair<uint32_t, uint32_t> dimension_sizes();
        int get_current_format();

        const cv::Mat& get_cv_data();

        void convert_image_color_space(cv::ColorConversionCodes code);

        int get_channels();

        uint32_t get_size();
        uint32_t get_width();
        uint32_t get_height();
        
    };
}

#endif //HARPY_UTILITIES_IMAGES