#include "opencv_ptest/include/opencv2/ts/ts.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>

#include "skeleton_filter.hpp"

using namespace std;
using namespace perf;
using namespace cv;
using std::tr1::make_tuple;
using std::tr1::get;

//#define RUN_LOCAL
#ifdef RUN_LOCAL
#define TESTDATA_PATH "testdata/"
#else
#define TESTDATA_PATH "./bin/testdata/"
#endif

PERF_TEST(skeleton, ConvertColor_BGR2GRAY_BT709)
{
    Mat input = cv::imread(TESTDATA_PATH"sla.png");

	cv::Mat src(input), dst(input);
	declare.in(src).out(dst);
	TEST_CYCLE()
	{
		ConvertColor_BGR2GRAY_BT709(src, dst);
	}

	SANITY_CHECK_NOTHING();
}

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

#define IMAGES testing::Values(std::string(TESTDATA_PATH"sla.png"),\
                               std::string(TESTDATA_PATH"page.png"),\
                               std::string(TESTDATA_PATH"schedule.png"))

typedef perf::TestBaseWithParam<std::string> testParams_t;;

PERF_TEST_P(testParams_t, skeletonize, IMAGES)
{
	cv::Mat src = cv::imread(GetParam());
	cv::Mat dst(src);
	declare.in(src).out(dst);
	TEST_CYCLE()
	{
		skeletonize(src, dst, false);
	}
	SANITY_CHECK_NOTHING();
}

PERF_TEST_P(testParams_t, ImageResize, IMAGES)
{
	cv::Mat src = cv::imread(GetParam());
	cv::Size newSize(src.rows / 2, src.cols / 2);
	cv::Mat dst(newSize, CV_8UC1);
	declare.in(src).out(dst);
	TEST_CYCLE()
	{
		ImageResize(src, dst, newSize);
	}
	SANITY_CHECK_NOTHING();
}
