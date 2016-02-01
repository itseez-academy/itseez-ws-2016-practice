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

TEST(skeleton, isImageHaveSameSizeAfterConvertFromBGRToGray)
{
    Mat bgr(5, 5, CV_8UC3);
    randu(bgr, Scalar::all(0), Scalar::all(255));

    Mat result;
    ConvertColor_BGR2GRAY_BT709(bgr, result);
 
	cv::Mat::MSize expectedSize(bgr.size);
	EXPECT_EQ(expectedSize, result.size);
}

TEST(skeleton, isImageHaveSameSizeAfterGuoHallThinning)
{
    Mat bgr(5, 5, CV_8UC3);
    randu(bgr, Scalar::all(0), Scalar::all(255));

    Mat grayImage;
	ConvertColor_BGR2GRAY_BT709(bgr, grayImage);
	Mat result;
    GuoHallThinning(grayImage, result);
 
	cv::Mat::MSize expectedSize(bgr.size);
	EXPECT_EQ(expectedSize, result.size);
}

TEST(skeleton, isImageHaveProperSizeAfterResize)
{
    Mat bgr(10, 10, CV_8UC1);
    randu(bgr, Scalar::all(0), Scalar::all(255));

	Mat result;
	cv::Size small_size(2, 2);
    ImageResize(bgr, result, small_size);
 
	EXPECT_EQ(small_size.height, result.size[0]);
	EXPECT_EQ(small_size.width, result.size[1]);
}

TEST(skeleton, isImageHaveSameColorAfterResize)
{
    Mat bgr(10, 10, CV_8UC1);
    randu(bgr, Scalar::all(0), Scalar::all(1));

	Mat result;
	cv::Size small_size(2, 2);
    ImageResize(bgr, result, small_size);
 
	double expectedMax, expectedMin;
	minMaxLoc(bgr, &expectedMin, &expectedMax);
	double max, min;
	minMaxLoc(result, &min, &max);

	EXPECT_EQ(expectedMin, expectedMax);
	EXPECT_EQ(min, max);
	EXPECT_EQ(expectedMin, min);
}

