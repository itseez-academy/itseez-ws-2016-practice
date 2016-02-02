#include "opencv_ptest/include/opencv2/ts/ts.hpp"

#include <iostream>
#include "skeleton_filter.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace perf;
using namespace cv;
using std::tr1::make_tuple;
using std::tr1::get;

#define MAT_SIZES  ::perf::szVGA, ::perf::sz720p, ::perf::sz1080p
#define IMAGES testing::Values( std::string("testdata\\sla.png"),\
                                std::string("testdata\\page.png"),\
                                std::string("testdata\\schedule.png") )

typedef perf::TestBaseWithParam<std::string> ImageName;
typedef perf::TestBaseWithParam<Size> Size_Only;

PERF_TEST_P(Size_Only, ImageResize, testing::Values(MAT_SIZES))
{
    Size sz = GetParam();
    Size sz_to(sz.width / 2, sz.height / 2);

    Mat src(sz, CV_8UC1), dst(Size(sz_to), CV_8UC1);
    declare.in(src, WARMUP_RNG).out(dst).iterations(100).time(30);

    TEST_CYCLE()
    {
        ImageResize(src, dst, sz_to);
    }

    SANITY_CHECK(dst, 1 + 1e-6);
}

PERF_TEST_P(ImageName, ConvertColor_BGR2GRAY_BT709, IMAGES)
{
     Mat input = imread(GetParam());
	 Mat dst(input.size(),CV_8SC1);
	 declare.in(input, WARMUP_RNG).out(dst).time(30);

	 TEST_CYCLE()
	 {
		 ConvertColor_BGR2GRAY_BT709(input,dst);
	 }

	 SANITY_CHECK(dst, 1 + 1e-6);
}

PERF_TEST_P(ImageName, skeleton_save, IMAGES)
{
    Mat input = imread(GetParam());
	Mat dst(input.size(), CV_8UC1);
	declare.in(input, WARMUP_RNG).out(dst).time(20);
	TEST_CYCLE()
	{
		skeletonize(input,dst,false);
	}
	SANITY_CHECK(dst, 1 + 1e-6);
}

PERF_TEST_P(ImageName, skeleton_not_save,IMAGES)
{
    Mat input = imread(GetParam());
	Mat dst(input.size(), CV_8UC1);
	declare.in(input, WARMUP_RNG).out(dst).time(20);
	TEST_CYCLE()
	{
		skeletonize(input,dst,true);
	}
	SANITY_CHECK(dst, 1 + 1e-6);
}

