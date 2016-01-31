#include "opencv_ptest/include/opencv2/ts/ts.hpp"

#include <iostream>

#include "skeleton_filter.hpp"

using namespace std;
using namespace perf;
using namespace cv;
using std::tr1::make_tuple;
using std::tr1::get;


#define TYPICAL_MAT_SIZES_ABS  ::perf::szVGA, ::perf::sz720p, ::perf::sz1080p
#define TYPICAL_MATS_ABS       testing::Combine( , testing::Values( TYPICAL_MAT_TYPES_ABS) )

typedef perf::TestBaseWithParam<Size> Size_Only;

PERF_TEST_P(Size_Only, ImageResize, testing::Values(TYPICAL_MAT_SIZES_ABS))
{
    Size sz = GetParam();
    // cout << sz << endl;

    Size sz_to(sz.width / 2, sz.height / 2);

    cv::Mat src(sz, CV_8UC1), dst(Size(sz_to), CV_8UC1);
    declare.in(src, WARMUP_RNG).out(dst);

    TEST_CYCLE()
    {
        // resize(src, dst, sz_to);
        ImageResize(src, dst, sz_to);
    }

    SANITY_CHECK(dst, 1 + 1e-6);
}
