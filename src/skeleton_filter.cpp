#include "skeleton_filter.hpp"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

void skeletonize(const cv::Mat &input, cv::Mat &output, bool save_images)
{
    TS(total);

    TS(imwrite_0);
    if (save_images) cv::imwrite("0-input.png", input);
    TE(imwrite_0);

    // Convert to grayscale
    TS(converting_to_grayscale);
    cv::Mat gray_image;
    ConvertColor_BGR2GRAY_BT709(input, gray_image);
    if (save_images)
    {
        cv::imwrite("1-convertcolor.png", gray_image);
    }
    TE(converting_to_grayscale);

    // Downscale input image
    TS(downscale_img);
    cv::Mat small_image;
    cv::Size small_size(input.cols / 1.5, input.rows / 1.5);
    ImageResize(gray_image, small_image, small_size);
    if (save_images)
    {
        TS(imwrite_downscale);
        cv::imwrite("2-resize.png", small_image);
        TE(imwrite_downscale);
    }
    TE(downscale_img);
    // Binarization and inversion
    TS(binarization_img);
    cv::threshold(small_image, small_image, 128, 255, cv::THRESH_BINARY_INV);
    if (save_images)
    {
        cv::imwrite("3-threshold.png", small_image);
    }
    TE(binarization_img);
    // Thinning
    TS(thinning_img);
    cv::Mat thinned_image;
    GuoHallThinning(small_image, thinned_image);
    if (save_images) 
    {
        cv::imwrite("4-thinning.png", thinned_image);
    }
    TE(thinning_img);
    // Back inversion
    TS(back_inversion_img);
    output = 255 - thinned_image;
    if (save_images) 
    {
        cv::imwrite("5-output.png", output);
    }
    TE(back_inversion_img);
    TE(total);
}
