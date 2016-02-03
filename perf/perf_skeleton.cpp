#include "opencv_ptest/include/opencv2/ts/ts.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>

#include "skeleton_filter.hpp"

using namespace std;
using namespace perf;
using namespace cv;
using std::tr1::make_tuple;
using std::tr1::get;

//
// Test(s) for the ImageResize function
//

#define MAT_SIZES  ::perf::szVGA, ::perf::sz720p, ::perf::sz1080p, ::perf::sz1440p, ::perf::sz4320p, ::perf::sz5MP

typedef perf::TestBaseWithParam<Size> Size_Only;

PERF_TEST_P(Size_Only, ImageResize, testing::Values(MAT_SIZES))
{
    Size sz = GetParam();
    Size sz_to(sz.width / 2, sz.height / 2);

    cv::Mat src(sz, CV_8UC1), dst(Size(sz_to), CV_8UC1);
    declare.in(src, WARMUP_RNG).out(dst);
    declare.time(30);

    TEST_CYCLE()
    {
        ImageResize(src, dst, sz_to);
    }

    SANITY_CHECK(dst, 1 + 1e-6);
}
PERF_TEST_P(Size_Only, ImageResizeSokolov, testing::Values(MAT_SIZES))
{
    Size sz = GetParam();
    Size sz_to(sz.width / 2, sz.height / 2);

    cv::Mat src(sz, CV_8UC1), dst(Size(sz_to), CV_8UC1);
    declare.in(src, WARMUP_RNG).out(dst);
    declare.time(30);

    TEST_CYCLE()
    {
        ImageResizeSokolov(src, dst, sz_to);
    }

    SANITY_CHECK(dst, 1 + 1e-6);
}
