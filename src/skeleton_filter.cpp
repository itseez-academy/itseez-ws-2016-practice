#include "skeleton_filter.hpp"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

void skeletonize(const cv::Mat &input, cv::Mat &output, bool save_images)
{
    TS(total);

    //TS(imwrite_0);
    if (save_images) cv::imwrite("0-input.png", input);
    //TE(imwrite_0);
	
    // Convert to grayscale
	//TS(ConvertColor_BGR2GRAY_BT709);
    cv::Mat gray_image;
    ConvertColor_BGR2GRAY_BT709(input, gray_image);
	//TE(ConvertColor_BGR2GRAY_BT709);
    if (save_images) { TS(1convertcolor_png); cv::imwrite("1-convertcolor.png", gray_image); TE(1convertcolor_png); }

    // Downscale input image
	//TS(ImageResize);
    cv::Mat small_image;
    cv::Size small_size(input.cols / 1.5, input.rows / 1.5);
    ImageResize(gray_image, small_image, small_size);
	//TE(ImageResize);
    if (save_images) { TS(2resize_png); cv::imwrite("2-resize.png", small_image); TE(2resize_png); }

    // Binarization and inversion
	//TS(threshold);
    cv::threshold(small_image, small_image, 128, 255, cv::THRESH_BINARY_INV);
	//TE(threshold);
    if (save_images) { TS(3threshold_png); cv::imwrite("3-threshold.png", small_image); TE(3threshold_png); }

    // Thinning
	//TS(GuoHallThinning);
    cv::Mat thinned_image;
    GuoHallThinning(small_image, thinned_image);
	//TE(GuoHallThinning);
	if (save_images) { TS(4thinning_png); cv::imwrite("4-thinning.png", thinned_image); TE(4thinning_png); }

	//TS(imwrite_5);
    // Back inversion
    output = 255 - thinned_image;
    if (save_images) cv::imwrite("5-output.png", output);
	//TE(imwrite_5);

    TE(total);
}
