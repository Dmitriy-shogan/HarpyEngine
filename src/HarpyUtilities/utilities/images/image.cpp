#include <images/image.h>
#include <logger/harpy_little_error.h>

void shift_bit(int& num, int bit){
    num ^= 1<<bit;
}

void harpy::utilities::image::read_image(std::string path)
{
    cv_image = imread(path, cv::IMREAD_UNCHANGED);
    switch(cv_image.channels()){
        case 1:
            current_format = R;
            break;
        case 3:
            current_format = BGR;
            break;
        case 4:
            current_format = BGRA;
    }
}

void harpy::utilities::image::save_image(std::string path, std::string filename)
{
    //imwrite(path + filename, cv_image);
}

const cv::Mat& harpy::utilities::image::get_cv_data()
{return cv_image;
}

harpy::utilities::image::image(std::string path) {
    read_image(std::move(path));
}

std::pair<uint32_t, uint32_t> harpy::utilities::image::dimension_sizes() {
    return {cv_image.cols, cv_image.rows};
}

void harpy::utilities::image::convert_image_color_space(cv::ColorConversionCodes code) {
    switch (code) {
        case cv::COLOR_BGR2BGRA: {
            if (current_format == RGB || current_format == BGR) {
                cv::cvtColor(cv_image, cv_image, code, 4);
                shift_bit(current_format, 0);
            } else goto error;
        }
        case cv::COLOR_BGRA2BGR: {
            if (current_format == RGBA || current_format == BGRA) {
                cv::cvtColor(cv_image, cv_image, code, 3);
                shift_bit(current_format, 0);
            } else goto error;
        }
        case cv::COLOR_BGR2RGBA: {
            if (current_format == BGR || current_format == RGB) {
                cv::cvtColor(cv_image, cv_image, code, 4);
                shift_bit(current_format, 0);
                shift_bit(current_format, 4);
            } else goto error;
        }
        case cv::COLOR_RGBA2BGR: {
            if (current_format == RGBA || current_format == BGRA) {
                cv::cvtColor(cv_image, cv_image, code, 3);
                shift_bit(current_format, 0);
                shift_bit(current_format, 4);
            } else goto error;
        }
        case cv::COLOR_BGR2RGB: {
            if (current_format == RGB || current_format == BGR) {
                cv::cvtColor(cv_image, cv_image, code, 3);
                shift_bit(current_format, 4);
            } else goto error;
        }
        case cv::COLOR_BGRA2RGBA: {
            if (current_format == RGBA || current_format == BGRA) {
                cv::cvtColor(cv_image, cv_image, code, 4);
                shift_bit(current_format, 4);
            } else goto error;
        }
        case cv::COLOR_BGR2GRAY: {
            if (current_format == BGR) {
                cv::cvtColor(cv_image, cv_image, code, 1);
                shift_bit(current_format, 5);
            } else goto error;
        }
        case cv::COLOR_RGB2GRAY: {
            if (current_format == RGB) {
                cv::cvtColor(cv_image, cv_image, code, 1);
                shift_bit(current_format, 5);
            } else goto error;
        }
        case cv::COLOR_GRAY2BGR: {
            if (current_format & is_GRAY) {
                cv::cvtColor(cv_image, cv_image, code, 3);
                current_format = BGR;
            } else goto error;
        }
        case cv::COLOR_GRAY2BGRA: {
            if (current_format == RGBA || current_format == BGRA) {
                cv::cvtColor(cv_image, cv_image, code, 4);
                current_format = BGRA;
            } else goto error;
        }
        case cv::COLOR_BGRA2GRAY: {
            if (current_format == RGBA || current_format == BGRA) {
                cv::cvtColor(cv_image, cv_image, code, 1);
                shift_bit(current_format, 5);
            } else goto error;
        }
        case cv::COLOR_RGBA2GRAY: {
            if (current_format == RGBA || current_format == BGRA) {
                cv::cvtColor(cv_image, cv_image, code, 1);
                shift_bit(current_format, 5);
            } else goto error;
        }
        default:
        error:
            throw harpy_little_error("Wrong image code conversion used (or not supported yet)");
    }
}

int harpy::utilities::image::get_current_format() {
    return current_format;
}

int harpy::utilities::image::get_channels() {
    return cv_image.channels();
}

uint32_t harpy::utilities::image::get_size() {
    return cv_image.total() * cv_image.elemSize();
}

uint32_t harpy::utilities::image::get_width() {
    return cv_image.cols;
}

uint32_t harpy::utilities::image::get_height() {
    return cv_image.rows;
}

