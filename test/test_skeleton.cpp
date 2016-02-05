#include <gtest/gtest.h>

#include "skeleton_filter.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <iterator>

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


// Task 2.1
TEST(skeleton, test_resize_equal)
{
    // Arrange
    Mat bgr(5, 5, CV_8UC1);
    randu(bgr, Scalar::all(0), Scalar::all(255));
    const cv::Size SIZE_TO_SET(5, 5);

    // Act
    Mat result;
    ImageResize(bgr, result, SIZE_TO_SET);

    // Assert
//    std::cout << bgr << std::endl;
//    std::cout << result << std::endl;
//    std::cout << maxDifference(bgr, result) << std::endl;
    EXPECT_LT(maxDifference(bgr, result), 0.00001f);
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

    const uchar color(142);
    bgr = color;

    const cv::Size SIZE_TO_SET(8, 13);

    // Act
    Mat result;
    ImageResize(bgr, result, SIZE_TO_SET);

    double min, max;
    Point minLoc, maxLoc;

    minMaxLoc(result, &min, &max, &minLoc, &maxLoc);

    // Assert
    EXPECT_EQ(color, result.at<uchar>(minLoc.x,minLoc.y));
    EXPECT_EQ(color, result.at<uchar>(maxLoc.x,minLoc.y));
}


// Task 4
TEST(skeleton, test_cvtcolor_dont_change_onecolored_image)
{
    // Arrange
    Mat bgr(5, 5, CV_8UC3, CV_RGB(120, 142, 111)); // R G B

    // Act
    Mat result;
    ConvertColor_BGR2GRAY_BT709(bgr, result);

    uchar newColor = result.at<uchar>(0, 0);

    // Assert

    double min, max;
    Point minLoc, maxLoc;

    minMaxLoc(result, &min, &max, &minLoc, &maxLoc);

    // Assert
    EXPECT_EQ(newColor, result.at<uchar>(minLoc.x, minLoc.y));
    EXPECT_EQ(newColor, result.at<uchar>(maxLoc.x, minLoc.y));
}

// Task 5
TEST(skeleton, test_guo_hall_black_pixel_count)
{
    // Arrange
    Mat bgr(5, 5, CV_8UC1); // One color type
    randu(bgr, Scalar::all(0), Scalar::all(200));

    // Act
    Mat result;
    GuoHallThinning_optimized(bgr, result);

    // Assert
/*
    std::cout << "********* Source **********" << std::endl;
    std::copy(bgr.begin<uchar>(), bgr.end<uchar>(), std::ostream_iterator<uchar>(std::cout, " "));
    std::cout << std::endl;
    std::cout << "Black size: " << std::count(bgr.begin<uchar>(), bgr.end<uchar>(), 0) << std::endl;
    std::cout << "White size: " << std::count(bgr.begin<uchar>(), bgr.end<uchar>(), 255) << std::endl;

    std::cout << "********* Result **********" << std::endl;
    std::copy(result.begin<uchar>(), result.end<uchar>(), std::ostream_iterator<uchar>(std::cout, " "));
    std::cout << std::endl;
    std::cout << "Black size: " << std::count(result.begin<uchar>(), result.end<uchar>(), 0) << std::endl;
    std::cout << "White size: " << std::count(result.begin<uchar>(), result.end<uchar>(), 255) << std::endl;
*/

    ASSERT_LE(std::count(bgr.begin<uchar>(), bgr.end<uchar>(), 0),
              std::count(result.begin<uchar>(), result.end<uchar>(), 0));

}


TEST(skeleton, Thinning)
{
std::cout << "Iterations test" << std::endl;
    int n = 21;
    Size sz = Size(n, n);

    cv::Mat image(sz, CV_8UC1, 255);

    int iter = 0;

    for (int i = 1; i < n-1 - iter; i += 7) {
        for (int j = iter * 4; j < n-1 - ((iter + 1) % 2) * 4; j+=2) {
            image.at<uchar>(i, j) = 0;
            image.at<uchar>(i-1, j+1) = 0;
            image.at<uchar>(i+1, j+1) = 0;
        }
        iter = (iter + 1) % 2;
    }
    image.at<uchar>(3, n-1) = 0;
    image.at<uchar>(4, n-1) = 0;
    image.at<uchar>(5, n-1) = 0;
    image.at<uchar>(6, n-1) = 0;


//cv::RNG rng(234231412);
//rng.fill(image, CV_8UC1, 0, 255);
//cv::threshold(image, image, 240, 255, cv::THRESH_BINARY_INV);

    cv::Mat gold;
    GuoHallThinning(image, gold);

    cv::Mat thinned_image;
    TS(Thinning);
    GuoHallThinning_optimized(image, thinned_image);
    TE(Thinning);


    cv::Mat diff;
    cv::absdiff(thinned_image, gold, diff);
    ASSERT_EQ(0, cv::countNonZero(diff));
}

/*
TEST(skeleton, Thinning_square)
{
    Size sz(100, 100);

    cv::Mat image(sz, CV_8UC1);

    cv::RNG rng(234231412);
    rng.fill(image, CV_8UC1, 0, 255);
    cv::threshold(image, image, 240, 255, cv::THRESH_BINARY_INV);

    cv::Mat gold; GuoHallThinning(image, gold);

    cv::Mat thinned_image;

    GuoHallThinning_optimized_sq(image, thinned_image);

    cv::Mat diff; cv::absdiff(thinned_image, gold, diff);
    ASSERT_EQ(0, cv::countNonZero(diff));
}*/
