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

TEST(skeleton, ConvertColor_received_size_is_equal_to_sourse_one)
{
	// Arrange
    Mat bgr(100, 100, CV_8UC3);
    randu(bgr, Scalar::all(0), Scalar::all(255));

    // Act
    Mat result;
    ConvertColor_BGR2GRAY_BT709(bgr, result);

	EXPECT_EQ(bgr.cols, result.cols);
	EXPECT_EQ(bgr.rows, result.rows);
}

TEST(skeleton, GuoHallThinning_received_size_is_equal_to_sourse_one)
{
	// Arrange
    Mat bgr(100, 100, CV_8UC1);
    randu(bgr, Scalar::all(0), Scalar::all(255));

    // Act
    Mat result;
    GuoHallThinning(bgr, result);

	EXPECT_EQ(bgr.cols, result.cols);
	EXPECT_EQ(bgr.rows, result.rows);
}

TEST(skeleton, ImageResize_result_is_determined_sizes)
{
	// Arrange
    Mat bgr(100, 100, CV_8UC1);
    randu(bgr, Scalar::all(0), Scalar::all(255));

    // Act
    Mat result;
	Size size(50, 50);
    ImageResize(bgr, result, size);

	EXPECT_EQ(50, result.rows);
	EXPECT_EQ(50, result.cols);
}