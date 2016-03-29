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

static uchar encode(uchar p2, uchar p3, uchar p4, uchar p5, uchar p6, uchar p7, uchar p8, uchar p9)
{
	return p2 * 1 +
		   p3 * 2 +
		   p4 * 4 +
		   p5 * 8 +
		   p6 * 16 +
		   p7 * 32 +
		   p8 * 64 +
		   p9 * 128;
}
static void decode(uchar code, uchar& p2, uchar& p3, uchar& p4, uchar& p5, uchar& p6, uchar& p7, uchar& p8, uchar& p9)
{
	p2 = (bool)(code & 1);
	p3 = (bool)(code & 2);
	p4 = (bool)(code & 4);
	p5 = (bool)(code & 8);
	p6 = (bool)(code & 16);
	p7 = (bool)(code & 32);
	p8 = (bool)(code & 64);
	p9 = (bool)(code & 128);
}

static void GuoHallIteration_optimized(cv::Mat& im, std::vector<uchar> table, cv::Mat& prev, cv::Mat& status)
{
    cv::Mat marker = cv::Mat::zeros(im.size(), CV_8UC1);

    for (int i = 1; i < im.rows-1; i++)
    {
        for (int j = 1; j < im.cols-1; j++)
        {
            if (im.at<uchar>(i,j) != 0 && status.at<uchar>(i,j) != 0)
			{
                uchar p9 = im.at<uchar>(i-1, j-1);
                uchar p2 = im.at<uchar>(i-1, j);
                uchar p3 = im.at<uchar>(i-1, j+1);
                uchar p8 = im.at<uchar>(i, j-1);
                uchar p4 = im.at<uchar>(i, j+1);
                uchar p7 = im.at<uchar>(i+1, j-1);
                uchar p6 = im.at<uchar>(i+1, j);
                uchar p5 = im.at<uchar>(i+1, j+1);

				int code = encode(p2,p3,p4,p5,p6,p7,p8,p9);
                marker.at<uchar>(i,j) = table[code];

                uchar newActualValue = im.at<uchar>(i,j) & !table[code];

                if (newActualValue != prev.at<uchar>(i,j))
                {
                    status.at<uchar>(i-1, j-1) = 1;
                    status.at<uchar>(i-1, j) = 1;
                    status.at<uchar>(i-1, j+1) = 1;
                    status.at<uchar>(i, j-1) = 1;
                    status.at<uchar>(i, j+1) = 1;
                    status.at<uchar>(i+1, j-1) = 1;
                    status.at<uchar>(i+1, j) = 1;
                    status.at<uchar>(i+1, j+1) = 1;
                }
                status.at<uchar>(i,j) = 0;
			}
        }
    }

    im &= ~marker;
}

void fillTableIter0(std::vector<uchar>& table)
{
	int iter = 0;
	for (int i=0; i<256; i++)
	{
		uchar p2;
		uchar p3;
		uchar p4;
		uchar p5;
		uchar p6;
		uchar p7;
		uchar p8;
		uchar p9;
		// i - code
		decode(i, p2,p3,p4,p5,p6,p7,p8,p9);
		int C  = (!p2 & (p3 | p4)) + (!p4 & (p5 | p6)) +
						 (!p6 & (p7 | p8)) + (!p8 & (p9 | p2));
		int N1 = (p9 | p2) + (p3 | p4) + (p5 | p6) + (p7 | p8);
		int N2 = (p2 | p3) + (p4 | p5) + (p6 | p7) + (p8 | p9);
		int N  = N1 < N2 ? N1 : N2;
		int m  = iter == 0 ? ((p6 | p7 | !p9) & p8) : ((p2 | p3 | !p5) & p4);

		if (C == 1 && (N >= 2 && N <= 3) & (m == 0))
			table[i] = 1;			
	}
}
void fillTableIter1(std::vector<uchar>& table)
{
	int iter = 1;
	for (int i=0; i<256; i++)
	{
		uchar p2 = 0;
		uchar p3 = 0;
		uchar p4 = 0;
		uchar p5 = 0;
		uchar p6 = 0;
		uchar p7 = 0;
		uchar p8 = 0;
		uchar p9 = 0;
		// i - code
		decode(i, p2,p3,p4,p5,p6,p7,p8,p9);
		int C  = (!p2 & (p3 | p4)) + (!p4 & (p5 | p6)) +
						 (!p6 & (p7 | p8)) + (!p8 & (p9 | p2));
		int N1 = (p9 | p2) + (p3 | p4) + (p5 | p6) + (p7 | p8);
		int N2 = (p2 | p3) + (p4 | p5) + (p6 | p7) + (p8 | p9);
		int N  = N1 < N2 ? N1 : N2;
		int m  = iter == 0 ? ((p6 | p7 | !p9) & p8) : ((p2 | p3 | !p5) & p4);

		if (C == 1 && (N >= 2 && N <= 3) & (m == 0))
			table[i] = 1;			
	}
}

void GuoHallThinning_optimized(const cv::Mat& src, cv::Mat& dst)
{
    CV_Assert(CV_8UC1 == src.type());

    dst = src / 255;

    cv::Mat prev;
    cv::Mat diff;
    cv::Mat changeGrid = ~(cv::Mat::zeros(src.size(), CV_8UC1));
    dst.copyTo(prev);

	std::vector<uchar> tableIter0(256, 0);
	std::vector<uchar> tableIter1(256, 0);
	fillTableIter0(tableIter0);
	fillTableIter1(tableIter1);

    do
    {
        GuoHallIteration_optimized(dst, tableIter0, prev, changeGrid);
        dst.copyTo(prev);
        GuoHallIteration_optimized(dst, tableIter1, prev, changeGrid);
        cv::absdiff(dst, prev, diff);
        dst.copyTo(prev);
    }
    while (cv::countNonZero(diff) > 0);

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
