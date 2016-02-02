#include "opencv_ptest/include/opencv2/ts/ts.hpp"

#include <iostream>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "skeleton_filter.hpp"

using namespace std;
using namespace perf;
using namespace cv;
using std::tr1::make_tuple;
using std::tr1::get;

//
// Test(s) for the ConvertColor_BGR2GRAY_BT709 function
//

#define IMAGES testing::Values( std::string("./bin/testdata/sla.png"),\
    std::string("./bin/testdata/page.png"),\
    std::string("./bin/testdata/schedule.png") )

typedef perf::TestBaseWithParam<std::string> ImageName;

PERF_TEST_P(ImageName, ConvertColor_BGR2GRAY_BT709, IMAGES)
{
    //Get parameters
    Mat input = imread(GetParam());
    Mat dst(input.size(), CV_8UC1);
    //declaring input, output and how many iterations should we do
    declare.in(input, WARMUP_RNG).out(dst).iterations(100);
    TEST_CYCLE()
    {
        ConvertColor_BGR2GRAY_BT709(input, dst);
    }
    //Regression check
    SANITY_CHECK(dst, 1 + 1e-6);
}

//
// Test(s) for the ImageResize function
//

#define MAT_SIZES  ::perf::szVGA, ::perf::sz720p, ::perf::sz1080p

typedef perf::TestBaseWithParam<Size> Size_Only;

PERF_TEST_P(Size_Only, ImageResize, testing::Values(MAT_SIZES))
{
    Size sz = GetParam();
    Size sz_to(sz.width / 2, sz.height / 2);

    Mat src(sz, CV_8UC1), dst(Size(sz_to), CV_8UC1);
    declare.in(src, WARMUP_RNG).out(dst).iterations(100);

    TEST_CYCLE()
    {
        ImageResize(src, dst, sz_to);
    }

    SANITY_CHECK(dst, 1 + 1e-6);
}

//
// Test(s) for the skeletonize function without save
//


PERF_TEST_P(ImageName, skeletonize_without_save, IMAGES)
{
    Mat input = imread(GetParam());
    Mat dst(input.size(), CV_8UC1);
    //declaring input, output and how many iterations should we do
    declare.in(input, WARMUP_RNG).out(dst).iterations(100).time(30);
    TEST_CYCLE()
    {
        skeletonize(input, dst, false);
    }
    //Regression check
    SANITY_CHECK(dst, 1 + 1e-6);
}

//
// Test(s) for the skeletonize function with save
//


PERF_TEST_P(ImageName, skeletonize_with_save, IMAGES)
{
    Mat input = imread(GetParam());
    Mat dst(input.size(), CV_8UC1);
    //declaring input, output and how many iterations should we do
    declare.in(input, WARMUP_RNG).out(dst).iterations(100).time(1000);
    TEST_CYCLE()
    {
        skeletonize(input, dst, true);
    }
    //Regression check
    SANITY_CHECK(dst, 1 + 1e-6);
}