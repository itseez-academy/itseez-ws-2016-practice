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

PERF_TEST(skeleton, ConvertColor_BGR2GRAY_BT709_sla)
{
	//Mat input = cv::imread("testdata/sla.png");
	Mat input = cv::imread("./bin/testdata/sla.png"); //TRAVIS
	Mat source(input);
	Mat destination(input);

    declare.in(source).out(destination);
	declare.time(30);
    TEST_CYCLE()
    {
        ConvertColor_BGR2GRAY_BT709(source, destination);
    }

    SANITY_CHECK_NOTHING();
 }

//
// Test(s) for the ImageResize function
//

#define MAT_SIZES  ::perf::szVGA, ::perf::sz720p, ::perf::sz1080p

typedef perf::TestBaseWithParam<Size> Size_Only;

PERF_TEST(skeleton, perf_ImageResize_sla)
{
	//Mat input = cv::imread("testdata/sla.png");
	Mat input = cv::imread("./bin/testdata/sla.png"); //TRAVIS
	ConvertColor_BGR2GRAY_BT709(input, input);

	Size newSize(input.size().width, input.size().height);
	Mat source(input);
	Mat destination(Size(newSize), CV_8UC1);

    declare.in(source).out(destination);
    TEST_CYCLE()
    {
		ImageResize(source, destination, newSize);
    }

    SANITY_CHECK_NOTHING();
 }

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

#define IMAGES testing::Values( std::string("./bin/testdata/sla.png"),\
                                std::string("./bin/testdata/page.png"),\
                                std::string("./bin/testdata/schedule.png") )

typedef perf::TestBaseWithParam<std::string> ImageName;

PERF_TEST_P(ImageName, skeletonize, IMAGES)
{
    Mat input = cv::imread(GetParam());
	Mat dst(input);
	declare.in(input).out(dst);
	TEST_CYCLE()
    {
        skeletonize(input, dst, false);
    }
	SANITY_CHECK_NOTHING();
}
