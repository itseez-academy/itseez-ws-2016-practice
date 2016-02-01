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

TEST(skeleton, same_sizes)
{
	Mat input(10, 10, CV_8UC3);
	
    randu(input, Scalar::all(0), Scalar::all(255));
	Mat dst(10, 10, CV_8UC3);
	ConvertColor_BGR2GRAY_BT709(input,dst);
	Mat final(10, 10, CV_8UC3);
	GuoHallThinning(dst,final);
	EXPECT_EQ(input.cols,final.cols);
	EXPECT_EQ(input.rows,final.rows);

}

TEST(skeleton, resizefunc_size_test)
{
	Mat input(10, 10, CV_8UC3);
	randu(input, Scalar::all(0), Scalar::all(255));
	ConvertColor_BGR2GRAY_BT709(input, input);
	Mat final(input.cols/2, input.rows/2, CV_8UC3);
	Size finalsize(input.cols / 2, input.rows / 2);
	ImageResize(input, final, finalsize);
	EXPECT_EQ(input.cols / 2,final.cols);
	EXPECT_EQ(input.rows / 2,final.rows);
}

TEST(skeleton,resizefunc_color_test)
{
	cv::Mat input(100, 100, CV_8UC1);
	input.setTo(Scalar(5));
	cv::Mat final(input.cols/2,input.rows/2,CV_8UC1);
	cv::Size finalsize(input.cols/2,input.rows/2);
	ImageResize(input,final,finalsize);
	bool flag = true;
	for(int i = 0; i < final.rows; i++)
		for(int j = 0; j < final.cols; j++)
		{
			if (final.at<uchar>(i,j) != 5)
				flag = false;
		}
	EXPECT_EQ(true,flag);
}

TEST(skeleton, bgr2grayfunc_color_test)
{
	cv::Scalar col(100,150,200);
	cv::Mat input(100,100,CV_8UC3,col);
	cv::Mat final(100,100,CV_8UC1);
	ConvertColor_BGR2GRAY_BT709(input, final);
	bool flag = true;
	int val = final.at<uchar>(0,0);
	for(int i = 0; i < final.rows; i++)
		for(int j = 0; j < final.cols; j++)
			if (final.at<uchar>(i,j) != val)
				flag = false;
	EXPECT_EQ(true,flag);
}
