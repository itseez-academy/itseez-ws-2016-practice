#include <gtest/gtest.h>

#include "skeleton_filter.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

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

//dummy test
TEST(skeleton, 2_plus_2_equals_4)
{
	EXPECT_EQ(4, 2 + 2);
}

//checking for input & output image size equality of BGR2GRAY_BT709
TEST(skeleton, check_image_sz_BGR2GRAY_BT709){
	// Arrange
    Mat bgr(10, 10, CV_8UC3);
    randu(bgr, Scalar::all(0), Scalar::all(255));

     // Act
    Mat result;
    ConvertColor_BGR2GRAY_BT709(bgr, result);

    // Assert

	EXPECT_EQ(bgr.size, result.size);
}

//checking for input & output image size equality of GuoHallThinning
TEST(skeleton, check_image_sz_GuoHallThinning){
	// Arrange
    Mat bgr(10, 10, CV_8UC1);
    randu(bgr, Scalar::all(0), Scalar::all(255));

     // Act
    Mat result;
    GuoHallThinning(bgr, result);

    // Assert

	EXPECT_EQ(bgr.size, result.size);
}

//checking for output image size equality to desired output image size prduced by ImageResize
TEST(skeleton, ImageResize_gettingDesiredSize){
    // Arrange
    Mat image(50, 50, CV_8UC1);
    //filling image with solid red color
    randu(image, Scalar::all(0), Scalar::all(255));
    Size sz(image.cols/3,image.rows/3);

     // Act
    Mat result;
    ImageResize(image, result, sz);

    // Assert

    EXPECT_EQ(result.size(), sz);
}

//checking for input & output image size equality of ImageResize in case mono color image supplied
TEST(skeleton, ImageResize_size_monoColorInput){
    // Arrange
    Mat image(50, 50, CV_8UC1);
    //filling image with solid red color
    image.setTo(cv::Scalar(123));
    Size sz(image.cols,image.rows);

     // Act
    Mat result;
    ImageResize(image, result, sz);

    // Assert

    EXPECT_EQ(image.size, result.size);
}

//checking whether the output image contsists of the pixels of the same color
//in case of 3 channel mono color input image being supplied
TEST(skeleton, ConvertColor_BGR2GRAY_BT709_OutSolidColor){
    // Arrange
    Mat image(50, 50, CV_8UC3);
    //filling image with solid red color
    image.setTo(cv::Scalar(0,0,255));
    Size sz(image.cols,image.rows);

     // Act
    Mat result,mcResult(50, 50, CV_8UC1),mdiff;
    ConvertColor_BGR2GRAY_BT709(image, result);

    //picking up the first pixel color and setting it to image two compare with.
    //one expects the ouput image of ConvertColor_BGR2GRAY_BT709 function
    //to contain all the pixels of the same color, so the first pixel must represent the whole output image.
    uchar color = result.at<uchar>(Point(0,0));

    mcResult.setTo(color);

    cv::compare(mcResult,result,mdiff,cv::CMP_NE);

    int nz = cv::countNonZero(mdiff);

    // Assert

    EXPECT_EQ(nz, 0);
}

//checking that blak pixels quantity in input <= than those in output
TEST(skeleton, BGR2GRAY_BT709_blPixelsCount){
    Mat image(500, 500, CV_8UC1);
    randu(image, Scalar::all(0), Scalar::all(255));

    // Act
    Mat result;
    GuoHallThinning(image, result);

    uchar color = 0;

    Mat whImage(500, 500, CV_8UC1),mdiff;
    whImage.setTo(color);

    //counting non black pixels.
    //I presume that if quantity of non black pixels after substraction is greater than those
    //in the resulting image, than the quantity of black pixels is greater in the last one
    cv::compare(whImage,image,mdiff,cv::CMP_NE);
    int nzInput = cv::countNonZero(mdiff);

    mdiff.deallocate();

    cv::compare(whImage,result,mdiff,cv::CMP_NE);
    int nzResult = cv::countNonZero(mdiff);

    // Assert

    EXPECT_GE(nzInput,nzResult);
}
