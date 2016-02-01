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

TEST(skeleton, size_matches_ConvertColor)
{
    Mat source(10, 10, CV_8UC3);
    Mat result;
    ConvertColor_BGR2GRAY_BT709(source, result);
    EXPECT_EQ(source.size(), result.size());
}

TEST(skeleton, size_matches_GuoHallThinning)
{
    Mat source(10, 10, CV_8UC1);
    Mat result;
    GuoHallThinning(source, result);
    EXPECT_EQ(source.size(), result.size());
}

TEST(skeleton, size_changed_ImageResizing)
{
    Mat source(10, 10, CV_8UC1);
    Mat result;
    Size expected_size(source.cols / 2, source.rows / 2);
    ImageResize(source, result, expected_size);
    EXPECT_EQ(expected_size, result.size());
}

TEST(skeleton, color_matches_ImageResizing)
{
    Mat source(10, 10, CV_8UC1, Scalar(128));
    Mat expected(5, 5, CV_8UC1, Scalar(128));
    Mat result;
    ImageResize(source, result, Size(source.cols / 2, source.rows / 2));
    Mat difference_mat = abs(expected - result);
    int difference_sum = countNonZero(difference_mat);
    EXPECT_EQ(0, difference_sum);
}


TEST(skeleton, similar_colors_ConvertColor)
{
   Mat source(10, 10, CV_8UC3, Scalar(56, 86, 230));
   Mat result;
   ConvertColor_BGR2GRAY_BT709(source, result);
   Scalar expected_color(result.at<uchar>(0, 0));
   Mat expected_color_mat(10, 10, CV_8UC1, expected_color);
   Mat difference_mat = abs(expected_color_mat - result);
   int difference_sum = countNonZero(difference_mat);
   EXPECT_EQ(0, difference_sum);
}

TEST(skeleton, black_count_GuoHallThinning)
{
    Mat source(10, 10, CV_8UC1);
    randu(source, Scalar(0), Scalar(255));
    threshold(source, source, 128, 255, cv::THRESH_BINARY);
    int source_black_count = source.total() - countNonZero(source);
    Mat result;
    GuoHallThinning(source, result);
    int result_black_count = result.total() - countNonZero(result);
    EXPECT_TRUE(source_black_count <= result_black_count);
}