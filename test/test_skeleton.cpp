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

TEST(skeleton, cvtcolor_does_not_change_size)
{
	//Arrange
	Mat bgr(5, 5, CV_8UC3);
	randu(bgr, Scalar::all(0), Scalar::all(255));
	
	//Act
	Mat xyz;
	cvtColor(bgr, xyz, CV_BGR2XYZ);
		
	//Assert
	EXPECT_EQ(bgr.size, xyz.size);
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

TEST(skeleton, imgresize_return_reqiured_size)
{
	// Arrange
    Mat image(5, 5, CV_8UC1);
    randu(image, Scalar(0), Scalar(255));
    Size sz(image.cols / 2.5, image.rows / 2.5);

    // Act
    Mat result;
    ImageResize(image, result, sz);

    // Assert
	EXPECT_EQ(result.size(), sz);
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

TEST(skeleton, imgresize_does_not_change_color)
{
    // Arrange
	srand(0);
	uchar color;
	Mat image(5, 5, CV_8UC1, (color = rand() % 255));
    Size sz(image.cols / 2, image.rows / 2);

    // Act
    Mat result;
    ImageResize(image, result, sz);

    // Assert
	bool condition = true;

	for (int i = 0; i < result.rows; i++)
		for (int j = 0; j < result.cols; j++)
			condition = condition && result.ptr(i, j)[0] == color;
    EXPECT_EQ(condition, true);
}

TEST(skeleton, thinning_does_not_change_size)
{
	//Arrange
	Mat bgr(5, 5, CV_8UC1);
	randu(bgr, Scalar::all(0), Scalar::all(255));
	
	//Act
	Mat xyz;
	GuoHallThinning(bgr, xyz);
		
	//Assert
	EXPECT_EQ(bgr.size, xyz.size);
}