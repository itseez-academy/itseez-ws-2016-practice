#include "opencv_ptest/include/opencv2/ts/ts.hpp"

#include <iostream>

#include "skeleton_filter.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace perf;
using namespace cv;
using std::tr1::make_tuple;
using std::tr1::get;

//
// Test(s) for the ConvertColor_BGR2GRAY_BT709 function
//
typedef perf::TestBaseWithParam<std::string> ImageName;

#define IMAGES testing::Values( std::string("./bin/testdata/sla.png"),\
                                std::string("./bin/testdata/page.png"),\
                                std::string("./bin/testdata/schedule.png"))

PERF_TEST_P(ImageName, cvt_color, IMAGES)
{
    Mat input = cv::imread(GetParam());

    Mat out;

    TEST_CYCLE() {
        ConvertColor_BGR2GRAY_BT709(input, out);
    }

    SANITY_CHECK_NOTHING();
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

    cv::Mat src(sz, CV_8UC1), dst(Size(sz_to), CV_8UC1);
    declare.in(src, WARMUP_RNG).out(dst);

    TEST_CYCLE()
    {
        ImageResize(src, dst, sz_to);
    }

    SANITY_CHECK(dst, 1 + 1e-6);
}

//
// Test(s) for the skeletonize function
//

PERF_TEST_P(ImageName, skeletonize, IMAGES)
 {
     Mat input = cv::imread(GetParam());

     Mat out;

     TEST_CYCLE() {
         skeletonize(input, out, false);
     }

     SANITY_CHECK_NOTHING();
 }


//
// Test(s) for the ConvertColor_BGR2GRAY_BT709 function
//

PERF_TEST_P(Size_Only, guo_hall, testing::Values(MAT_SIZES))
{
    // Mat input = cv::imread(GetParam());

    Mat chb(GetParam(), CV_8UC1); // One color type
    randu(chb, Scalar::all(0), Scalar::all(255));

    // Timeout without treshold and resize
    // ConvertColor_BGR2GRAY_BT709(input, chb);

    Mat out;

    TEST_CYCLE() {
        GuoHallThinning(chb, out);
    }

    SANITY_CHECK_NOTHING();
}
