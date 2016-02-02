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


#define PATH "./bin/testdata"

//
// Test(s) for the ConvertColor_BGR2GRAY_BT709 function
//

PERF_TEST(skeleton, ConvertColor_BGR2GRAY_BT709)
{
	Mat input = cv::imread(PATH"/sla.png");
	Mat result(input.size(), CV_8UC1);
	declare.in(input).out(result);

	TEST_CYCLE()
	{
		ConvertColor_BGR2GRAY_BT709(input, result);
	}
	SANITY_CHECK(result, 1 + 1e-6);
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


#define IMAGES testing::Values( std::string(PATH"/sla.png"),\
                                std::string(PATH"/page.png"),\
                                std::string(PATH"/schedule.png") )


typedef perf::TestBaseWithParam<std::string> ImageName;

PERF_TEST_P(ImageName, skeletonize_without_saving, IMAGES)
{
	Mat input = cv::imread(GetParam());
	Mat result(input.size(), CV_8UC1);
	declare.in(input, WARMUP_RNG).out(result).time(100).iterations(100);

	TEST_CYCLE()
    {
        skeletonize(input, result, false);
    }

    SANITY_CHECK(result, 1 + 1e-6);
}

