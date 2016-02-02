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
    cv::Mat gray_image;
	TS(convert_0);
    ConvertColor_BGR2GRAY_BT709(input, gray_image);
    if (save_images) cv::imwrite("1-convertcolor.png", gray_image);
	TE(convert_0);

    // Downscale input image
    cv::Mat small_image;
    cv::Size small_size(input.cols / 1.5, input.rows / 1.5);
	TS(resize_0);
    ImageResize(gray_image, small_image, small_size);
    if (save_images) cv::imwrite("2-resize.png", small_image);
	TE(resize_0);

    // Binarization and inversion
    cv::threshold(small_image, small_image, 128, 255, cv::THRESH_BINARY_INV);
    if (save_images) cv::imwrite("3-threshold.png", small_image);

    // Thinning
    cv::Mat thinned_image;
	TS(thinning_0);
    GuoHallThinning(small_image, thinned_image);
	if (save_images) cv::imwrite("4-thinning.png", thinned_image);
	TE(thinning_0);

    // Back inversion
    output = 255 - thinned_image;
    if (save_images) cv::imwrite("5-output.png", output);

    TE(total);
}
