#include <gtest/gtest.h>

#include "skeleton_filter.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>

using namespace cv;

TEST(skeleton, cvtcolor_matches_opencv)
{
    // Arrange
    Mat bgr(5, 5, CV_8UC3);
    randu(bgr, Scalar::all(0), Scalar::all(255));
    Mat gray;

    // Act
    ConvertColor_BGR2GRAY_BT709(bgr, gray);

    // Assert
    Mat reference_gray;
    cvtColor(bgr, reference_gray, CV_BGR2GRAY);
    // std::cout << reference_gray - gray;
    EXPECT_EQ(0, countNonZero(reference_gray - gray));
}

TEST(skeleton, resize_matches_opencv)
{
    // Arrange
    Mat image(10, 10, CV_8UC1);
    randu(image, Scalar(0), Scalar(255));
    Mat small;
    Size sz(image.cols / 2, image.rows / 2);

    // Act
    ImageResize(image, small, sz);

    // Assert
    Mat reference_small;
    resize(image, reference_small, sz);
    Mat diff;
    absdiff(reference_small, small, diff);
    double maximum;
    minMaxLoc(diff, 0, &maximum);
    EXPECT_LT(maximum, 2);
}
