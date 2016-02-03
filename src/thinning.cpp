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

static void GuoHallIteration_optimized(cv::Mat& im, int iter, uchar* mas0, uchar* mas1)
{
    cv::Mat marker = cv::Mat::zeros(im.size(), CV_8UC1);

    for (int i = 1; i < im.rows-1; i++)
    {
        for (int j = 1; j < im.cols-1; j++)
        {
			if (im.at<uchar>(i,j)>0)
			{
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

				if (iter==0) {marker.at<uchar>(i,j) = mas0[code];}
				if (iter==1) {marker.at<uchar>(i,j) = mas1[code];}
			}
        }
    }
    im &= ~marker;
}
 
//char arr_0[255]; char arr_1[255];

void MakeMatrix(uchar* mas0, uchar* mas1)
{
	uchar p[10];
	for (int code = 0; code < 256; code++)
	{
		p[2] = code & 1;
		p[3] = code & 2;
		p[4] = code & 4;
		p[5] = code & 8;
		p[6] = code & 16;
		p[7] = code & 32;
		p[8] = code & 64;
		p[9] = code & 128;
		for (int k=0; k<10; k++)
		{
			if (p[k]>0) {p[k]=1;}
		}

		int iter=1;

		int C  = (!p[2] & (p[3] | p[4])) + (!p[4] & (p[5] | p[6])) +
						 (!p[6] & (p[7] | p[8])) + (!p[8] & (p[9] | p[2]));
		int N1 = (p[9] | p[2]) + (p[3] | p[4]) + (p[5] | p[6]) + (p[7] | p[8]);
		int N2 = (p[2] | p[3]) + (p[4] | p[5]) + (p[6] | p[7]) + (p[8] | p[9]);
		int N  = N1 < N2 ? N1 : N2;
		//int m  = iter == 0 ? ((p[6] | p[7] | !p[9]) & p[8]) : ((p[2] | p[3] | !p[5]) & p[4]);
		int m_is_0 = ((p[6] | p[7] | !p[9]) & p[8]);
		int m_is_1 = ((p[2] | p[3] | !p[5]) & p[4]);
		if (C == 1 && (N >= 2 && N <= 3) & (m_is_0 == 0))
			{mas0[code] = 1;} else {mas0[code] = 0;}
		if (C == 1 && (N >= 2 && N <= 3) & (m_is_1 == 0))
			{mas1[code] = 1;} else {mas1[code] = 0;}
	}
}

void GuoHallThinning_optimized(const cv::Mat& src, cv::Mat& dst)
{
    CV_Assert(CV_8UC1 == src.type());

    dst = src / 255;

    cv::Mat prev = cv::Mat::zeros(src.size(), CV_8UC1);
    cv::Mat diff;

	uchar arr_0[256]; uchar arr_1[256];
	MakeMatrix(arr_0, arr_1);
	int l=0;
    do
    {
        GuoHallIteration_optimized(dst, 0, arr_0, arr_1);
        GuoHallIteration_optimized(dst, 1, arr_0, arr_1);
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
