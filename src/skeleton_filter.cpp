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
	TS(ConvertColor_BGR2GRAY_BT709);
    ConvertColor_BGR2GRAY_BT709(input, gray_image);
	TE(ConvertColor_BGR2GRAY_BT709);
	TS(imwrite_1);
    if (save_images) cv::imwrite("1-convertcolor.png", gray_image);
	TE(imwrite_1);

    // Downscale input image
    cv::Mat small_image;
    cv::Size small_size(input.cols / 1.5, input.rows / 1.5);
	TS(ImageResize);
    ImageResize(gray_image, small_image, small_size);
	TE(ImageResize);
	TS(imwrite_2);
    if (save_images) cv::imwrite("2-resize.png", small_image);
	TE(imwrite_2);

    // Binarization and inversion
	TS(threshold);
    cv::threshold(small_image, small_image, 128, 255, cv::THRESH_BINARY_INV);
	TE(threshold);
	TS(imwrite_3);
    if (save_images) cv::imwrite("3-threshold.png", small_image);
	TE(imwrite_3);

    // Thinning
    cv::Mat thinned_image;
	TS(GuoHallThinning);
    GuoHallThinning(small_image, thinned_image);
	TE(GuoHallThinning);
	TS(imwrite_4);
    if (save_images) cv::imwrite("4-thinning.png", thinned_image);
	TE(imwrite_4);

    // Back inversion
    output = 255 - thinned_image;
	TS(imwrite_5);
    if (save_images) cv::imwrite("5-output.png", output);
	TE(imwrite_5);

    TE(total);
}
