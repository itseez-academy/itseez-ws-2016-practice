#include "skeleton_filter.hpp"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "iostream"
#include "fstream"
#include "iomanip"

void skeletonize(const cv::Mat &input, cv::Mat &output, bool save_images)
{
	std::ofstream time_logger;
	time_logger.open("../timelog.csv", std::fstream::out | std::fstream::app);

    TS(total);

    TS(imwrite_0);
    if (save_images) cv::imwrite("0-input.png", input);
    time_logger << TE(imwrite_0);

    // Convert to grayscale
	cv::Mat gray_image;
	TS(convert_color_bgr2gray_bt709);
    ConvertColor_BGR2GRAY_BT709(input, gray_image);
	time_logger << TE(convert_color_bgr2gray_bt709);
	TS(imwrite_1);
    if (save_images) cv::imwrite("1-convertcolor.png", gray_image);
	time_logger << TE(imwrite_1);
	
    // Downscale input image
	cv::Mat small_image;
    cv::Size small_size(input.cols / 1.5, input.rows / 1.5);
	TS(imgresize);
    ImageResize(gray_image, small_image, small_size);
	time_logger << TE(imgresize);
	TS(imwrite_2);
    if (save_images) cv::imwrite("2-resize.png", small_image);
	time_logger << TE(imwrite_2);
	
    // Binarization and inversion
	TS(threshold);
    cv::threshold(small_image, small_image, 128, 255, cv::THRESH_BINARY_INV);
	time_logger << TE(threshold);
	TS(imwrite_3);
    if (save_images) cv::imwrite("3-threshold.png", small_image);
	time_logger << TE(imwrite_3);
	
    // Thinning
    cv::Mat thinned_image;
	TS(thinning);
    GuoHallThinning(small_image, thinned_image);
    time_logger << TE(thinning);
	TS(imwrite_4);
	if (save_images) cv::imwrite("4-thinning.png", thinned_image);
	time_logger << TE(imwrite_4);

    // Back inversion
	output = 255 - thinned_image;
	TS(imwrite_5);
    if (save_images) cv::imwrite("5-output.png", output);
	time_logger << TE(imwrite_5);
	
	time_logger << TE(total) << std::endl;
	time_logger.close();
}
