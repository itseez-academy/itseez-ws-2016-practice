#include "opencv_ptest/include/opencv2/ts/ts.hpp"
#include "opencv2/opencv.hpp"

#include <iostream>

#include "skeleton_filter.hpp"

using namespace std;
using namespace perf;
using namespace cv;
using std::tr1::make_tuple;
using std::tr1::get;


#define MAT_SIZES  ::perf::szVGA, ::perf::sz720p, ::perf::sz1080p

typedef perf::TestBaseWithParam<Size> Size_Only;

//
// Test(s) for the ConvertColor_BGR2GRAY_BT709 function
//

PERF_TEST_P(Size_Only, ConvertColor_BGR2GRAY_BT709, testing::Values(MAT_SIZES))
{
    Size sz = GetParam();
    Mat src(sz, CV_8UC3);
    Mat dst(sz, CV_8UC1);

    declare.in(src, WARMUP_RNG).out(dst);

    TEST_CYCLE()
    {
        ConvertColor_BGR2GRAY_BT709(src, dst);
    }

    SANITY_CHECK_NOTHING();
}

//
// Test(s) for the ImageResize function
//

PERF_TEST_P(Size_Only, ImageResize, testing::Values(MAT_SIZES))
{
    Size sz = GetParam();
    Size sz_to(sz.width / 2, sz.height / 2);

    cv::Mat src(sz, CV_8UC1);
    cv::Mat dst(Size(sz_to), CV_8UC1);
    declare.in(src, WARMUP_RNG).out(dst);

    cv::RNG rng(234231412);
    rng.fill(src, CV_8UC1, 0, 255);

    TEST_CYCLE()
    {
        ImageResize(src, dst, sz_to);
    }

    SANITY_CHECK(dst);
}

//
// Test(s) for the skeletonize function
//

#define IMAGES testing::Values( std::string("./bin/testdata/sla.png"),\
                                std::string("./bin/testdata/page.png"),\
                                std::string("./bin/testdata/schedule.png") )

typedef perf::TestBaseWithParam<std::string> ImageName;

PERF_TEST_P(ImageName, skeletonize, IMAGES)
{
    Mat input = cv::imread(GetParam());
    Mat output(input.size(), CV_8UC1);

    declare.in(input).out(output);

    TEST_CYCLE()
    {
        skeletonize(input, output, false);
    }

    SANITY_CHECK_NOTHING();
}

void Invert(const Mat& src, Mat& dst)
{
    for (int y = 0; y < src.rows; ++y)
    {
        for (int x = 0; x < src.cols; ++x)
        {
            dst.at<uchar>(y, x) = 255 - src.at<uchar>(y, x);
        }
    }
}

PERF_TEST_P(Size_Only, Invert, testing::Values(MAT_SIZES))
{
    Size sz = GetParam();
    Mat src(sz, CV_8UC1);
    Mat dst(sz, CV_8UC1);

    declare.in(src, WARMUP_RNG).out(dst);

    TEST_CYCLE()
    {
        Invert(src, dst);
    }

    SANITY_CHECK(dst);
}

//
// Test(s) for the Thinning function
//

PERF_TEST_P(Size_Only, Thinning, testing::Values(MAT_SIZES))
{
    Size sz = GetParam();

    cv::Mat image(sz, CV_8UC1);
    declare.in(image, WARMUP_RNG).out(image);
    declare.time(40);

    cv::RNG rng(234231412);
    rng.fill(image, CV_8UC1, 0, 255);
    cv::threshold(image, image, 240, 255, cv::THRESH_BINARY_INV);

    cv::Mat gold; GuoHallThinning(image, gold);

    cv::Mat thinned_image;
    TEST_CYCLE()
    {
        GuoHallThinning_optimized(image, thinned_image);
    }

    cv::Mat diff; cv::absdiff(thinned_image, gold, diff);
    ASSERT_EQ(0, cv::countNonZero(diff));

    SANITY_CHECK(image);
}
