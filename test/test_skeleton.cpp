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

TEST(skeleton, check_size_ConvertColor_BGR2GRAY_BT709)
{
    Mat source(9, 9, CV_8UC3);
    randu(source, Scalar::all(0), Scalar::all(255));

    Mat result;
    ConvertColor_BGR2GRAY_BT709(source, result);

	EXPECT_EQ(source.size, result.size);
}

TEST(skeleton, check_size_GuoHallThinning)
{
    Mat source(9, 9, CV_8UC1);
    randu(source, Scalar::all(0), Scalar::all(255));

	cv::threshold(source, source, 128, 255, cv::THRESH_BINARY_INV);

    Mat result;
    GuoHallThinning(source, result);

	EXPECT_EQ(result.size, source.size);
}

TEST(skeleton, check_size_after_ImageResize)
{
    Mat source(9, 9, CV_8UC1);
    randu(source, Scalar::all(0), Scalar::all(255));


	Mat result;
	cv::Size newSize(10, 10);
	ImageResize(source, result, newSize);

	bool success = false;
	if(result.size[0] == newSize.width && result.size[1] == newSize.height)
	{
		success = true;
	}

	EXPECT_EQ(true, success);
}

TEST(skeleton, check_color_after_ImageResize)
{
	uchar color = 111;
    Mat source(9, 9, CV_8UC1, color);

	Mat result;
	cv::Size newSize(10, 10);
	ImageResize(source, result, newSize);

	bool success = true;
	for(int i = 0; i < newSize.width; ++i) 
	{
		for(int j = 0; j < newSize.height; ++j) 
		{
			if(source.at<uchar>(i, j) != color) 
			{
				success = false;
				break;
			}
		}
	}

	EXPECT_EQ(true, success);
}

TEST(skeleton, test_ConvertColor_BGR2GRAY_BT709_8UC3_plain)
{
	uchar color = 111;
	Mat source(9, 9, CV_8UC3, color);
    randu(source, Scalar::all(0), Scalar::all(255));

    Mat result;
    ConvertColor_BGR2GRAY_BT709(source, result);
	uchar resultColor = result.at<uchar>(0, 0);
	
	bool success = true;
	cv::Size resultSize = result.size;
	for(int i = 0; i < resultSize.width; ++i) 
	{
		for(int j = 0; j < resultSize.height; ++j) 
		{
			if(source.at<uchar>(i, j) != color) 
			{
				success = false;
				break;
			}
		}
	}
	
	EXPECT_EQ(true, success);
}

 TEST(skeleton, test_GuoHallThinning_number_of_black_pixels_not_less_source)
{
	Mat source(9, 9, CV_8UC1);
	randu(source, Scalar::all(0), Scalar::all(2));
	
	Mat result;
	GuoHallThinning(source, result);
	
	EXPECT_EQ(true, (countNonZero(result) < countNonZero(source)));
+}

