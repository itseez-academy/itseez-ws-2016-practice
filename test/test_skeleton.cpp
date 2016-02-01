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

// Task 1
TEST(skeleton, test_cvtcolor_dont_resize_image)
{
    // Arrange
    Mat bgr(5, 5, CV_8UC3);
    randu(bgr, Scalar::all(0), Scalar::all(255));

    // Act
    Mat result;
    ConvertColor_BGR2GRAY_BT709(bgr, result);

    // Assert
    // Looks like overkill...
    EXPECT_EQ(bgr.dims, result.dims);
    EXPECT_EQ(bgr.size, result.size);
}

TEST(skeleton, test_guo_hall_dont_resize_image)
{
    // Arrange
    Mat bgr(5, 5, CV_8UC1); // One color type
    randu(bgr, Scalar::all(0), Scalar::all(255));

    // Act
    Mat result;
    GuoHallThinning(bgr, result);

    // Assert
    EXPECT_EQ(bgr.dims, result.dims);
    EXPECT_EQ(bgr.size, result.size);
}

// Task 2
TEST(skeleton, test_resize_generates_correctly_sized_image)
{
    // Arrange
    Mat bgr(5, 5, CV_8UC1);
    randu(bgr, Scalar::all(0), Scalar::all(255));
    const cv::Size SIZE_TO_SET(8, 13);

    // Act
    Mat result;
    ImageResize(bgr, result, SIZE_TO_SET);

    // Assert
    EXPECT_EQ(SIZE_TO_SET, result.size());
}

// Task 3
TEST(skeleton, test_resize_dont_change_color_of_onecolored_image)
{
    // Arrange
    Mat bgr = Mat::ones(20, 20, CV_8UC1);

    const cv::Size SIZE_TO_SET(8, 13);

    // Act
    Mat result;
    ImageResize(bgr, result, SIZE_TO_SET);

    double min, max;
    Point minLoc, maxLoc;

    minMaxLoc(result, &min, &max, &minLoc, &maxLoc);

    // Assert
    ASSERT_EQ(1, result.at<uchar>(minLoc.x,minLoc.y));
    ASSERT_EQ(1, result.at<uchar>(maxLoc.x,minLoc.y));
}


