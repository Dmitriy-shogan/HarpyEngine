#include <images/image.h>

void harpy::utilities::images::image::read_image(std::string path)
{
    cv_image = imread(path, cv::IMREAD_UNCHANGED);
}

void harpy::utilities::images::image::save_image(std::string path, std::string filename)
{
    //imwrite(path + filename, cv_image);
}

const cv::Mat& harpy::utilities::images::image::get_cv_data()
{return cv_image;
}

