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
	TS(convert_to_grayscale);
    cv::Mat gray_image;
	TS(ConvertColor_BGR2GRAY_BT709_0);
    ConvertColor_BGR2GRAY_BT709(input, gray_image);
	TE(ConvertColor_BGR2GRAY_BT709_0);
	TS(imwrite_1);
    if (save_images) cv::imwrite("1-convertcolor.png", gray_image);
	TE(imwrite_1);
	TE(convert_to_grayscale);

    // Downscale input image
	TS(downscale_input_image);
    cv::Mat small_image;
    cv::Size small_size(input.cols / 1.5, input.rows / 1.5);
	TS(ImageResize_0);
    ImageResize(gray_image, small_image, small_size);
	TE(ImageResize_0);
	TS(imwrite_2);
    if (save_images) cv::imwrite("2-resize.png", small_image);
	TE(imwrite_2);
	TE(downscale_input_image);

    // Binarization and inversion
	TS(binarization_and_inversion);
	TS(threshold_0);
    cv::threshold(small_image, small_image, 128, 255, cv::THRESH_BINARY_INV);
	TE(threshold_0);
	TS(imwrite_3);
    if (save_images) cv::imwrite("3-threshold.png", small_image);
	TE(imwrite_3);
	TE(binarization_and_inversion);

    // Thinning
	TS(thinning);
    cv::Mat thinned_image;
	TS(GuoHallThinning_0);
    GuoHallThinning(small_image, thinned_image);
	TE(GuoHallThinning_0);
	TS(imwrite_4);
    if (save_images) cv::imwrite("4-thinning.png", thinned_image);
	TE(imwrite_4);
	TE(thinning);

    // Back inversion
	TS(back_inversion);
    output = 255 - thinned_image;
	TS(imwrite_5);
    if (save_images) cv::imwrite("5-output.png", output);
	TE(imwrite_5);
	TE(back_inversion);

    TE(total);
}
