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

TEST(skeleton, convt_size_test){
    // Arrange
    Mat bgr(5, 5, CV_8UC3);
    randu(bgr, Scalar::all(0), Scalar::all(255));

    // Act
    Mat result;
    ConvertColor_BGR2GRAY_BT709(bgr, result);

    // Assert
    EXPECT_EQ(bgr.size(), result.size());
}

TEST(skeleton, GHT_size_test){
    // Arrange
    Mat src(5, 5, CV_8UC1);
    randu(src, Scalar::all(0), Scalar::all(255));

    // Act
    Mat result;
    GuoHallThinning(src, result);

    // Assert
    EXPECT_EQ(src.size(), result.size());
}

TEST(skeleton, imres_size_test){
    // Arrange
    Mat src(10, 10, CV_8UC1);
    randu(src, Scalar::all(0), Scalar::all(255));

    // Act
    Mat result;
    Size sz(src.cols/2,src.rows/2);
    ImageResize(src, result, sz);

    // Assert
    EXPECT_EQ(sz, result.size());
}
TEST(skeleton, imres_color_test){
    // Arrange
    Mat src(10, 10, CV_8UC1);
    randu(src, Scalar::all(0), Scalar::all(255));

    // Act
    Mat result;
    Size sz(src.cols / 2, src.rows / 2);
    ImageResize(src, result, sz);

    // Assert
    EXPECT_EQ(CV_8UC1, result.type());
}

TEST(skeleton, convt_pixel_test){
    // Arrange
    Mat bgr(10, 10, CV_8UC3);
    randu(bgr, Scalar::all(0), Scalar::all(255));

    // Act
    Mat result;
    ConvertColor_BGR2GRAY_BT709(bgr, result);

    // Assert
    EXPECT_EQ(bgr.size(), result.size());
}

TEST(skeleton, GHT_pixel_test){
    // Arrange
    Mat src(15, 15, CV_8UC1);
    randu(src, Scalar::all(0), Scalar::all(255));
    size_t srccnt = 0;
    size_t rescnt = 0;

    // Act
    Mat result;
    GuoHallThinning(src, result);
    uchar black = 0;

    // Assert
    for (size_t i = 0; i < result.cols; i++){
        for (size_t j = 0; j < result.rows; j++)
        {
            if (result.at<uchar>(j, i) == black) rescnt++;
            if (src.at<uchar>(i, j) == black) srccnt++;
        }
    }
    EXPECT_GE(rescnt, srccnt);
}