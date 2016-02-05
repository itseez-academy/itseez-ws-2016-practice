#include "skeleton_filter.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

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

void createTable(uchar table[], int iter)
{
	uchar p2, p3, p4, p5, p6, p7, p8, p9;
	for(int i = 0; i < 256; i++)
	{
		p2 = 0;
		p3 = 0;
		p4 = 0;
		p5 = 0;
		p6 = 0;
		p7 = 0;
		p8 = 0;
		p9 = 0;
		table[i] = 0;
		if((i & 1) != 0)
			p2 = 1;
		if((i & 2) != 0)
			p3 = 1;
		if((i & 4) != 0)
			p4 = 1;
		if((i & 8) != 0)
			p5 = 1;
		if((i & 16) != 0)
			p6 = 1;
		if((i & 32) != 0)
			p7 = 1;
		if((i & 64) != 0)
			p8 = 1;
		if((i & 128) != 0)
			p9 = 1;

		int C  = (!p2 & (p3 | p4)) + (!p4 & (p5 | p6)) +
				(!p6 & (p7 | p8)) + (!p8 & (p9 | p2));
			int N1 = (p9 | p2) + (p3 | p4) + (p5 | p6) + (p7 | p8);
			int N2 = (p2 | p3) + (p4 | p5) + (p6 | p7) + (p8 | p9);
			int N  = N1 < N2 ? N1 : N2;
			int m  = iter == 0 ? ((p6 | p7 | !p9) & p8) : ((p2 | p3 | !p5) & p4);

			if (C == 1 && (N >= 2 && N <= 3) & (m == 0))
				table[i] = 1;
			//std::cout<<"table["<<i<<"]="<<table[i]<<std::endl;
	}
}


static void GuoHallIteration_optimized(cv::Mat& im, int iter)
{
	uchar table[256];
	createTable(table, iter);

	cv::Mat marker = cv::Mat::zeros(im.size(), CV_8UC1);

	for (int i = 1; i < im.rows-1; i++)
	{
		for (int j = 1; j < im.cols-1; j++)
		{
			if (im.at<uchar>(i,j) != 0)
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

				
				/*int C  = (!p2 & (p3 | p4)) + (!p4 & (p5 | p6)) +
					(!p6 & (p7 | p8)) + (!p8 & (p9 | p2));
				int N1 = (p9 | p2) + (p3 | p4) + (p5 | p6) + (p7 | p8);
				int N2 = (p2 | p3) + (p4 | p5) + (p6 | p7) + (p8 | p9);
				int N  = N1 < N2 ? N1 : N2;
				int m  = iter == 0 ? ((p6 | p7 | !p9) & p8) : ((p2 | p3 | !p5) & p4);
				
				if (C == 1 && (N >= 2 && N <= 3) & (m == 0))
					marker.at<uchar>(i,j) = 1;*/
				if(table[code] != 0)
					marker.at<uchar>(i,j) = 1;
				
			}
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

	do
	{
		GuoHallIteration_optimized(dst, 0);
		GuoHallIteration_optimized(dst, 1);
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
