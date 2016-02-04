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

TEST(skeleton, the_same_size_ConvertColor_BGR2GRAY_BT709)
{
	Mat image(20, 40, CV_8UC3);
	Mat result;
	
	ConvertColor_BGR2GRAY_BT709(image, result);
	
	EXPECT_EQ(image.cols, result.cols);
	EXPECT_EQ(image.rows, result.rows);
}

TEST(skeleton, the_same_size_GuoHallThinning)
{
	Mat image(100, 100, CV_8UC1);
	Mat result;

	GuoHallThinning(image, result);

	EXPECT_EQ(image.cols, result.cols);
	EXPECT_EQ(image.rows, result.rows);
}

TEST(skeleton, the_same_size_of_arg_ImageResize)
{
	Mat image(200, 100, CV_8UC1);
	Mat result;

	Size sz(image.cols / 2, image.rows / 2);
	ImageResize(image, result, sz);

	EXPECT_EQ(result.cols, sz.width);
	EXPECT_EQ(result.rows, sz.height);
}

TEST(skeleton, the_same_color_ImageResize)
{
	Mat image(200, 200, CV_8UC1, Scalar(30));
	Mat result;

	Size sz(image.cols / 2, image.rows / 2);
	ImageResize(image, result, sz);
	uchar color = image.at<uchar>(0, 0);

	for (int i = 0; i < sz.height; i++) {
		for (int j = 0; j < sz.width; j++) {
			EXPECT_EQ(result.at<uchar>(i, j), color);
		}
	}
}

TEST(skeleton, the_same_color_ConvertColor_BGR2GRAY_BT709)
{
	Scalar color = Scalar(40, 80, 120);
	Mat image(200, 200, CV_8UC3, color);
	Mat result;

	ConvertColor_BGR2GRAY_BT709(image, result);
	uchar chng_color = result.at<uchar>(0, 0);

	for (int i = 0; i < result.rows; i++) {
		for (int j = 0; j < result.cols; j++) {
			EXPECT_EQ(result.at<uchar>(i, j), chng_color);
		}
	}
}

