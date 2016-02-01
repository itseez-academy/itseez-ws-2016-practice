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

TEST(skeleton, test_multipy_2x2)
{
	EXPECT_EQ(2 * 2, 4);
}

TEST(skeleton, test_ConvertColor_BGR2GRAY_BT709_equalsSize)
{
	const Mat src(10, 10, CV_8UC3);
	Mat dst;
	ConvertColor_BGR2GRAY_BT709(src, dst);
	EXPECT_EQ(src.size, dst.size);
}

TEST(skeleton, test_ImageResize_correctSize)
{
	Mat dst;
	Size size(20, 15);
	ImageResize(Mat(10, 10, CV_8UC1), dst, size);
	EXPECT_EQ(dst.size[0], size.height);
	EXPECT_EQ(dst.size[1], size.width);
}

TEST(skeleton, test_ImageResize_singleColorSave)
{
	typedef unsigned char uchar;
	const uchar value = 145;
	Mat src(10, 10, CV_8UC1, value);
	Mat dst;
    Size size(5, 5); // this test fail when dst.size >= src.size.
	ImageResize(src, dst, size);
	double dstMin, dstMax;
	minMaxLoc(dst, &dstMin, &dstMax);
	EXPECT_EQ(static_cast<uchar>(dstMin), value);
	EXPECT_EQ(static_cast<uchar>(dstMax), value);
}

TEST(skeleton, test_ConvertColor_BGR2GRAY_BT709_singleColorSave)
{
	typedef unsigned char uchar;
	const uchar value = 145;
	Mat src(10, 10, CV_8UC3, value);
	Mat dst;
	ConvertColor_BGR2GRAY_BT709(src, dst);
	double dstMin, dstMax;
	minMaxLoc(dst, &dstMin, &dstMax);
	EXPECT_EQ(static_cast<uchar>(dstMin), static_cast<uchar>(dstMax));
}

TEST(skeleton, test_GuoHallThinning_blackPixelsCountGeSource)
{
	typedef unsigned char uchar;
	Mat src(10, 10, CV_8UC1);
	randu(src, 0, 2);
	Mat dst;
	GuoHallThinning(src, dst);
	EXPECT_EQ((countNonZero(dst) < countNonZero(src)), true);
}
