#include "skeleton_filter.hpp"
#include <opencv2/imgproc/imgproc.hpp>

static void GuoHallIteration(cv::Mat& im, int iter)
{
    cv::Mat marker = cv::Mat::zeros(im.size(), CV_8UC1);

    for (int i = 1; i < im.rows-1; i++)
    {
        for (int j = 1; j < im.cols-1; j++)
        {
            uchar p2 = im.at<uchar>(i-1, j);
            uchar p3 = im.at<uchar>(i-1, j+1);
            uchar p4 = im.at<uchar>(i, j+1);
            uchar p5 = im.at<uchar>(i+1, j+1);
            uchar p6 = im.at<uchar>(i+1, j);
            uchar p7 = im.at<uchar>(i+1, j-1);
            uchar p8 = im.at<uchar>(i, j-1);
            uchar p9 = im.at<uchar>(i-1, j-1);

            int C  = (!p2 & (p3 | p4)) + (!p4 & (p5 | p6)) +
                     (!p6 & (p7 | p8)) + (!p8 & (p9 | p2));
            int N1 = (p9 | p2) + (p3 | p4) + (p5 | p6) + (p7 | p8);
            int N2 = (p2 | p3) + (p4 | p5) + (p6 | p7) + (p8 | p9);
            int N  = N1 < N2 ? N1 : N2;
            int m  = iter == 0 ? ((p6 | p7 | !p9) & p8) : ((p2 | p3 | !p5) & p4);

            if (C == 1 && (N >= 2 && N <= 3) & (m == 0))
                marker.at<uchar>(i,j) = 1;
        }
    }

    im &= ~marker;
}

void GuoHallThinning(const cv::Mat& src, cv::Mat& dst)
{
    CV_Assert(CV_8UC1 == src.type());

    dst = src / 255;

    cv::Mat prev = cv::Mat::zeros(src.size(), CV_8UC1);
    cv::Mat diff;

    do
    {
        GuoHallIteration(dst, 0);
        GuoHallIteration(dst, 1);
        cv::absdiff(dst, prev, diff);
        dst.copyTo(prev);
    }
    while (cv::countNonZero(diff) > 0);

    dst *= 255;
}

//
// Place optimized version here
//

static uchar GetIndex(uchar p2, uchar p3, uchar p4, uchar p5, uchar p6, uchar p7, uchar p8, uchar p9)
{
	// Pixel neighbourhood structure
	// p9 p2 p3
	// p8 p1 p4
	// p7 p6 p5

	// Encode neghbourhood pixel values to byte
	uchar code = p2 * 1 +
		   p3 * 2 +
		   p4 * 4 +
		   p5 * 8 +
		   p6 * 16 +
		   p7 * 32 +
		   p8 * 64 +
		   p9 * 128;

	if (code == 15)
		printf("123");

	return code;
}

static uchar CountIJ(int iter, uchar p2, uchar p3, uchar p4, uchar p5, uchar p6, uchar p7, uchar p8, uchar p9, uchar *p23456789)
{
	uchar index = GetIndex(p2,p3,p4,p5,p6,p7,p8,p9);

	int res = p23456789[index];
	if (res != 2) 
		return res;

	res = 0;

	int C  = (!p2 & (p3 | p4)) + (!p4 & (p5 | p6)) +
			(!p6 & (p7 | p8)) + (!p8 & (p9 | p2));
	int N1 = (p9 | p2) + (p3 | p4) + (p5 | p6) + (p7 | p8);
	int N2 = (p2 | p3) + (p4 | p5) + (p6 | p7) + (p8 | p9);
	int N  = N1 < N2 ? N1 : N2;
	int m  = iter == 0 ? ((p6 | p7 | !p9) & p8) : ((p2 | p3 | !p5) & p4);

	if (C == 1 && (N >= 2 && N <= 3) & (m == 0))
		res = 1;

	p23456789[index] = res;
	return res;
}

static void GuoHallIteration_optimized(cv::Mat& im, int iter, uchar *p23456789)
{
    cv::Mat marker = cv::Mat::zeros(im.size(), CV_8UC1);

    for (int i = 1; i < im.rows-1; i++)
    {
        for (int j = 1; j < im.cols-1; j++)
        {
			if (im.at<uchar>(i, j) == 0) continue;

            uchar p2 = im.at<uchar>(i-1, j);
            uchar p3 = im.at<uchar>(i-1, j+1);
            uchar p4 = im.at<uchar>(i, j+1);
            uchar p5 = im.at<uchar>(i+1, j+1);
            uchar p6 = im.at<uchar>(i+1, j);
            uchar p7 = im.at<uchar>(i+1, j-1);
            uchar p8 = im.at<uchar>(i, j-1);
            uchar p9 = im.at<uchar>(i-1, j-1);

			marker.at<uchar>(i,j) = CountIJ(iter, p2,p3,p4,p5,p6,p7,p8,p9, p23456789);
        }
    }

    im &= ~marker;
}

static void FillP(uchar *p23456789, int iter)
{
	for (int code=0; code<256; code++)
	{
		uchar res = 0;

		uchar p2 = (bool)(code & 1);
		uchar p3 = (bool)(code & 2);
		uchar p4 = (bool)(code & 4);
		uchar p5 = (bool)(code & 8);
		uchar p6 = (bool)(code & 16);
		uchar p7 = (bool)(code & 32);
		uchar p8 = (bool)(code & 64);
		uchar p9 = (bool)(code & 128);

		if (code == 15)
			printf("123");

		int C  = (!p2 & (p3 | p4)) + (!p4 & (p5 | p6)) +
			(!p6 & (p7 | p8)) + (!p8 & (p9 | p2));
		int N1 = (p9 | p2) + (p3 | p4) + (p5 | p6) + (p7 | p8);
		int N2 = (p2 | p3) + (p4 | p5) + (p6 | p7) + (p8 | p9);
		int N  = N1 < N2 ? N1 : N2;
		int m  = iter == 0 ? ((p6 | p7 | !p9) & p8) : ((p2 | p3 | !p5) & p4);

		if (C == 1 && (N >= 2 && N <= 3) & (m == 0))
			res = 1;

		p23456789[code] = res;
	}
}

void GuoHallThinning_optimized(const cv::Mat& src, cv::Mat& dst)
{
    CV_Assert(CV_8UC1 == src.type());

    dst = src / 255;

    cv::Mat prev = cv::Mat::zeros(src.size(), CV_8UC1);
    cv::Mat diff;

	uchar *p23456789_0 = new uchar[256]; FillP(p23456789_0, 0);//for (int i=0; i<256; i++) p23456789_0[i] = 2;
	uchar *p23456789_1 = new uchar[256]; FillP(p23456789_1, 0);//for (int i=0; i<256; i++) p23456789_1[i] = 2;

    do
    {
        GuoHallIteration_optimized(dst, 0, p23456789_0);
        GuoHallIteration_optimized(dst, 1, p23456789_1);
        cv::absdiff(dst, prev, diff);
        dst.copyTo(prev);
    }
    while (cv::countNonZero(diff) > 0);

	//printf("DIFF:\n");
	//uchar *p23456789_00 = new uchar[256]; FillP(p23456789_00, 0);	
	//uchar *p23456789_11 = new uchar[256]; FillP(p23456789_11, 1);
	//for (int i=0; i<256; i++)
	//{
	//	if (p23456789_0[i]!=2 && p23456789_00[i] != p23456789_0[i]) printf("0) Wrong in %i: %i %i\n", i, +p23456789_0[i], +p23456789_00[i]);
	//	if (p23456789_1[i]!=2 && p23456789_11[i] != p23456789_1[i]) printf("1) Wrong in %i: %i %i\n", i, +p23456789_1[i], +p23456789_11[i]);
	//}
	//printf("END\n");

    dst *= 255;
}

//
// Sample performance report
//
//           Name of Test               base          1           2          1          2
//                                                                           vs         vs
//                                                                          base       base
//                                                                       (x-factor) (x-factor)
// Thinning::Size_Only::640x480      333.442 ms  216.775 ms  142.484 ms     1.54       2.34
// Thinning::Size_Only::1280x720     822.569 ms  468.958 ms  359.877 ms     1.75       2.29
// Thinning::Size_Only::1920x1080    2438.715 ms 1402.072 ms 1126.428 ms    1.74       2.16
