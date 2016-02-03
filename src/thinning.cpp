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

void getLookUpTable(uchar *&table, int iter)
{
	for (int i = 0; i < 256; i++)
	{
		uchar p2 = (bool)(i & 1);
		uchar p3 = (bool)(i & 2);
		uchar p4 = (bool)(i & 4);
		uchar p5 = (bool)(i & 8);
		uchar p6 = (bool)(i & 16);
		uchar p7 = (bool)(i & 32);
		uchar p8 = (bool)(i & 64);
		uchar p9 = (bool)(i & 128);
		int C  = (!p2 & (p3 | p4)) + (!p4 & (p5 | p6)) +
				 (!p6 & (p7 | p8)) + (!p8 & (p9 | p2));
		int N1 = (p9 | p2) + (p3 | p4) + (p5 | p6) + (p7 | p8);
		int N2 = (p2 | p3) + (p4 | p5) + (p6 | p7) + (p8 | p9);
		int N  = N1 < N2 ? N1 : N2;
		int m  = iter == 0 ? ((p6 | p7 | !p9) & p8) : ((p2 | p3 | !p5) & p4);

		if (C == 1 && (N >= 2 && N <= 3) & (m == 0))
			table[i] = 1;
		else table[i] = 0;
	}
}

static void GuoHallIteration_optimized(cv::Mat& im, uchar *&table)
{
    cv::Mat marker = cv::Mat::zeros(im.size(), CV_8UC1);

    for (int i = 1; i < im.rows-1; i++)
    {
        for (int j = 1; j < im.cols-1; j++)
        {
			if (im.at<uchar>(i,j) == 0) continue;
            uchar p2 = im.at<uchar>(i-1, j);
            uchar p3 = im.at<uchar>(i-1, j+1);
            uchar p4 = im.at<uchar>(i, j+1);
            uchar p5 = im.at<uchar>(i+1, j+1);
            uchar p6 = im.at<uchar>(i+1, j);
            uchar p7 = im.at<uchar>(i+1, j-1);
            uchar p8 = im.at<uchar>(i, j-1);
            uchar p9 = im.at<uchar>(i-1, j-1);

			uchar code = p2 * 1 +
						 p3 * 2 +
						 p4 * 4 +
						 p5 * 8 +
						 p6 * 16 +
						 p7 * 32 +
						 p8 * 64 +
						 p9 * 128;

			marker.at<uchar>(i,j) = table[code];
        }
    }
    im &= ~marker;
}

void GuoHallThinning_optimized(const cv::Mat& src, cv::Mat& dst)
{
    CV_Assert(CV_8UC1 == src.type());

    dst = src / 255;

    cv::Mat prev = cv::Mat::zeros(src.size(), CV_8UC1);
    cv::Mat diff;

	uchar* table0 = new uchar [256]; 
	uchar* table1 = new uchar [256];
	getLookUpTable(table0, 0);
	getLookUpTable(table1, 1);

    do
    {
        GuoHallIteration_optimized(dst, table0);
        GuoHallIteration_optimized(dst, table1);
        cv::absdiff(dst, prev, diff);
        dst.copyTo(prev);
    }
    while (cv::countNonZero(diff) > 0);

    dst *= 255;
	delete table0;
	delete table1;
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
