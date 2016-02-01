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

TEST(skeleton, image_size_equals_ConvertColor_BGR2GRAY_BT709)
{
    // Arrange
	int rows = 100, cols = 100;
    Mat bgr(rows, cols, CV_8UC3);
    randu(bgr, Scalar::all(0), Scalar::all(255));

    // Act
    Mat result;
    ConvertColor_BGR2GRAY_BT709(bgr, result);

    // Assert
	EXPECT_EQ(rows, result.rows);
	EXPECT_EQ(cols, result.cols);
}

TEST(skeleton, image_size_equals_GuoHallThinning)
{
    // Arrange
	int rows = 100, cols = 100;
    Mat bgr(rows, cols, CV_8UC1);
    randu(bgr, Scalar::all(0), Scalar::all(255));

    // Act
    Mat result;
    GuoHallThinning(bgr, result);

    // Assert
	EXPECT_EQ(rows, result.rows);
	EXPECT_EQ(cols, result.cols);
}

TEST(skeleton, ImageResize_change_size)
{
    // Arrange
	int rows0 = 100, cols0 = 100;
	int rows1 = 50, cols1 = 50;
    Mat bgr(rows0, cols0, CV_8UC1);
    randu(bgr, Scalar::all(0), Scalar::all(255));

    // Act
    Mat result;
	ImageResize(bgr, result, Size(rows1, cols1));

    // Assert
	EXPECT_EQ(rows1, result.rows);
	EXPECT_EQ(cols1, result.cols);
}

TEST(skeleton, ImageResize_mat_equals_with_input_CV_8UC1)
{
    // Arrange
	int rows = 1, cols = 1;
    Mat bgr(rows, cols, CV_8UC1, 12);
	//randu(bgr, Scalar::all(5), Scalar::all(50));	// BAD, but sometimes worked
	
    // Act
    Mat result;
	ImageResize(bgr, result, Size(rows, cols));

    // Assert	
	Mat diff = bgr - result;
	int nonZeroCount = countNonZero(diff);
	bool eq = nonZeroCount == 0;
	EXPECT_EQ(true, eq);
}