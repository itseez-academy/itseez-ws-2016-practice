#include "skeleton_filter.hpp"

#include "opencv2/imgproc/imgproc.hpp"

void skeletonize(const cv::Mat &input, cv::Mat &output)
{
    // Convert to grayscale
    cv::Mat gray_image;
    // cv::cvtColor(input, gray_image, CV_BGR2GRAY);
    ConvertColor_BGR2GRAY_BT709(input, gray_image);

    // Downscale input image
    cv::Mat small_image;
    cv::Size small_size(input.cols / 1.5, input.rows / 1.5);
    // cv::resize(gray_image, small_image, small_size);
    ImageResize(gray_image, small_image, small_size);

    // Binarization
    cv::threshold(small_image, small_image, 128, 255, cv::THRESH_BINARY_INV);

    // Thinning
    cv::Mat thinned_image;
    GuoHallThinning(small_image, thinned_image);

    output = thinned_image;
}
