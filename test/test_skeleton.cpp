#include <gtest/gtest.h>

#include "skeleton_filter.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>

using namespace cv;

int numberOfDifferentPixels(const cv::Mat &m1, const cv::Mat &m2)
{
    return countNonZero(m1 - m2);
}

int maxDifference(const cv::Mat &m1, const cv::Mat &m2)
{
    Mat diff;
    absdiff(m1, m2, diff);

    double max_diff;
    minMaxLoc(diff, 0, &max_diff);

    return (int)max_diff;
}

TEST(skeleton, cvtcolor_matches_opencv)
{
    // Arrange
    Mat bgr(5, 5, CV_8UC3);
    randu(bgr, Scalar::all(0), Scalar::all(255));

    // Act
    Mat result;
    ConvertColor_BGR2GRAY_BT709(bgr, result);

    // Assert
    Mat xyz;
    cvtColor(bgr, xyz, CV_BGR2XYZ);
    vector<Mat> planes;
    split(xyz, planes);
    Mat reference = planes[1];
    // std::cout << "Difference:\n" << reference - result << std::endl;
    EXPECT_EQ(0, numberOfDifferentPixels(reference, result));
}

TEST(skeleton, resize_matches_opencv)
{
    // Arrange
    Mat image(10, 10, CV_8UC1);
    randu(image, Scalar(0), Scalar(255));
    Size sz(image.cols / 2, image.rows / 2);

    // Act
    Mat result;
    ImageResize(image, result, sz);

    // Assert
    Mat reference;
    resize(image, reference, sz);
    // std::cout << "Difference:\n" << reference - result << std::endl;
    EXPECT_LT(maxDifference(reference, result), 2);
}

TEST(skeleton, 2_plus_2_equals_4)
{
	EXPECT_EQ(4, 2 + 2);
}

TEST(skeleton, 2GRAY_size_check)
{
	cv::Mat original(10, 10, CV_8UC3);
	cv::Mat gray_image;
	randu(original, Scalar::all(0), Scalar::all(255));
	ConvertColor_BGR2GRAY_BT709(original, gray_image);
	Size size_gray = gray_image.size();
	Size size_original = original.size();
	EXPECT_EQ(size_gray.height, size_original.height);
	EXPECT_EQ(size_gray.width, size_original.width);
}

TEST(skeleton, GuoHall_size_check)
{
	cv::Mat original(10, 10, CV_8UC1);
	cv::Mat guo_image;
	randu(original, Scalar(0), Scalar(255));
	cv::threshold(original, original, 128, 255, cv::THRESH_BINARY_INV);
	GuoHallThinning(original, guo_image);
	Size size_guo = guo_image.size();
	Size size_original = original.size();
	EXPECT_EQ(size_guo.height, size_original.height);
	EXPECT_EQ(size_guo.width, size_original.width);
}

TEST(skeleton, ImageResize_matches_input)
{
	Mat image(10, 20, CV_8UC1);
	randu(image, Scalar(0), Scalar(255));
	Size sz(image.cols / 2, image.rows / 2);
	Mat result;
	ImageResize(image, result, sz);
	Size size_new = result.size();
	EXPECT_EQ(size_new.height, sz.height);
	EXPECT_EQ(size_new.width, sz.width);
}

TEST(skeleton, resize_same_color_check)
{
	Mat image(10, 20, CV_8UC1);
	Mat image_small(5, 10, CV_8UC1);
	image = cv::Scalar(10, 110, 210);
	image_small = cv::Scalar(10, 110, 210);
	Size sz(image.cols / 2, image.rows / 2);
	Mat result;
	ImageResize(image, result, sz);
	Mat subtr = abs(result - image_small);
	int a = countNonZero(subtr);
	EXPECT_EQ(a, 0);
}

TEST(skeleton, 2GRAY_same_color_check)
{
	cv::Mat original(10, 10, CV_8UC3);
	cv::Mat gray_image;
	original = cv::Scalar(10, 110, 210);
	ConvertColor_BGR2GRAY_BT709(original, gray_image);
	cv::Mat dst = gray_image.reshape(1);
	uchar a = dst.at<uchar>(1, 1);
	cv::Mat exp(10, 10, CV_8UC1);
	exp = cv::Scalar(a);
	Mat subtr = abs(exp - dst);
	int ba = countNonZero(subtr);
	EXPECT_EQ(ba, 0);
}	

TEST(skeleton, GuoHall_black_check)
{
	cv::Mat original(10, 10, CV_8UC1);
	cv::Mat guo_image;
	randu(original, Scalar(0), Scalar(255));
	cv::threshold(original, original, 128, 255, cv::THRESH_BINARY_INV);
	GuoHallThinning(original, guo_image);
	
	int nz1 = countNonZero(original);
	int nz2 = countNonZero(guo_image);
	EXPECT_LT(nz2, nz1);
}


