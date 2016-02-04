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
    EXPECT_EQ(2 + 2, 4);
}

TEST(skeleton, cvt_color_match_size)
{
    Mat src(5, 5, CV_8UC3);
    randu(src, Scalar::all(0), Scalar::all(255));

    Mat dst;
    ConvertColor_BGR2GRAY_BT709(src, dst);

    EXPECT_EQ(src.size(), dst.size());
}

TEST(skeleton, thinning_match_size)
{
    Mat src(5, 5, CV_8UC1);
    randu(src, 0, 255);

    Mat dst;
    GuoHallThinning(src, dst);

    EXPECT_EQ(src.size(), dst.size());
}

TEST(skeleton, resize_match_size)
{
    Mat src(3, 3, CV_8UC1);
    randu(src, 0, 255);

    Mat dst;
    Size size;
    for (size.height = src.rows * 2; size.height > 0; --size.height)
    {
        for (size.width = src.cols * 2; size.width > 0; --size.width)
        {
            ImageResize(src, dst, size);
            ASSERT_EQ(dst.size(), size);
        }
    }
}

TEST(skeleton, resize_match_color)
{
    Mat src(3, 3, CV_8UC1);
    uchar color = 123;
    src.setTo(color);

    Mat dst;
    Size size;
    for (size.height = src.rows * 2; size.height > 0; --size.height)
    {
        for (size.width = src.cols * 2; size.width > 0; --size.width)
        {
            ImageResize(src, dst, size);
            ASSERT_EQ(countNonZero(dst != color), 0);
        }
    }
}

TEST(skeleton, cvt_color_match_monochrome)
{
    Mat src(5, 5, CV_8UC1);
    randu(src, Scalar::all(0), Scalar::all(255));

    Mat src_multi_channel;
    std::vector<Mat> mat_vec(3);
    mat_vec[0] = mat_vec[1] = mat_vec[2] = src;
    merge(mat_vec, src_multi_channel);

    Mat dst;
    ConvertColor_BGR2GRAY_BT709(src_multi_channel, dst);

    EXPECT_EQ(countNonZero(dst != src), 0);
}

TEST(skeleton, thinning_blacks_count)
{
    Mat src(5, 5, CV_8UC1);
    randu(src, 0, 255);

    Mat dst;
    GuoHallThinning(src, dst);

    EXPECT_LT(countNonZero(dst), countNonZero(src));
}

TEST(skeleton, optim_thinning_match_size)
{
    Mat src(5, 5, CV_8UC1);
    randu(src, 0, 255);

    Mat dst;
    GuoHallThinning_optimized(src, dst);

    EXPECT_EQ(src.size(), dst.size());
}

TEST(skeleton, optim_thinning_blacks_count)
{
    Mat src(5, 5, CV_8UC1);
    randu(src, 0, 255);

    Mat dst;
    GuoHallThinning_optimized(src, dst);

    EXPECT_LT(countNonZero(dst), countNonZero(src));
}
