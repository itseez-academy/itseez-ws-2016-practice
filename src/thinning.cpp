#include "skeleton_filter.hpp"
#include <opencv2/imgproc/imgproc.hpp>
//#include <vector>
//#include <map>
//#include <cmath>

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

static void GuoHallIteration_optimized1(cv::Mat& im, int iter)
{
    cv::Mat marker = cv::Mat::zeros(im.size(), CV_8UC1);

    for (int i = 1; i < im.rows-1; i++)
    {
        for (int j = 1; j < im.cols-1; j++)
        {
			if (im.at<uchar>(i, j))
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
    }

    im &= ~marker;
}

static uchar createByte(std::vector<uchar> p)
{
	uchar res = 0;
	int pow2 = 1;
	for(int i = 0; i < 8; i++)
	{
		res += p[i] * pow2;
		pow2 *= 2;
	}
	return res;
}
static std::vector<std::vector<uchar> > table(2,std::vector<uchar>(256,0));


static void createTable(int iter)
{
	for(int i = 0; i < 256 ;  i++)
	{
		uchar p2 = i & 1;
		uchar p3 = i & 2;
		uchar p4 = i & 4;
		uchar p5 = i & 8;
		uchar p6 = i & 16;
		uchar p7 = i & 32;
		uchar p8 = i & 64;
		uchar p9 = i & 128;

		p2 = (p2 != 0 ? 1 : 0);
		p3 = (p3 != 0 ? 1 : 0);
		p4 = (p4 != 0 ? 1 : 0);
		p5 = (p5 != 0 ? 1 : 0);
		p6 = (p6 != 0 ? 1 : 0);
		p7 = (p7 != 0 ? 1 : 0);
		p8 = (p8 != 0 ? 1 : 0);
		p9 = (p9 != 0 ? 1 : 0);
		
		int C  = (!p2 & (p3 | p4)) + (!p4 & (p5 | p6)) +
					(!p6 & (p7 | p8)) + (!p8 & (p9 | p2));
		int N1 = (p9 | p2) + (p3 | p4) + (p5 | p6) + (p7 | p8);
		int N2 = (p2 | p3) + (p4 | p5) + (p6 | p7) + (p8 | p9);
		int N  = N1 < N2 ? N1 : N2;
		int m  = iter == 0 ? ((p6 | p7 | !p9) & p8) : ((p2 | p3 | !p5) & p4);
		if (C == 1 && (N >= 2 && N <= 3) & (m == 0))
						table[iter][i] = 1;
	}
	
}







static bool writer = true;

static void writeTable(int iter)
{
	if (writer)
	{
		for(int i = 0; i <  256; i++)
			printf("%d",table[iter][i]);
		printf("\n");
	}
	writer = false;
	
}

static void GuoHallIteration_optimized(cv::Mat& im, int iter)
{
	
    cv::Mat marker = cv::Mat::zeros(im.size(), CV_8UC1);

    for (int i = 1; i < im.rows-1; i++)
    {
        for (int j = 1; j < im.cols-1; j++)
        {
			if (im.at<uchar>(i, j))
			{
				/*std::vector<uchar> p(8);
				p[0] = im.at<uchar>(i-1, j);
				p[1] = im.at<uchar>(i-1, j+1);
				p[2] = im.at<uchar>(i, j+1);
				p[3] = im.at<uchar>(i+1, j+1);
				p[4] = im.at<uchar>(i+1, j);
				p[5] = im.at<uchar>(i+1, j-1);
				p[6] = im.at<uchar>(i, j-1);
				p[7] = im.at<uchar>(i-1, j-1);*/
				uchar p2 = im.at<uchar>(i-1, j);
				uchar p3 = im.at<uchar>(i-1, j+1);
				uchar p4 = im.at<uchar>(i, j+1);
				uchar p5 = im.at<uchar>(i+1, j+1);
				uchar p6 = im.at<uchar>(i+1, j);
				uchar p7 = im.at<uchar>(i+1, j-1);
				uchar p8 = im.at<uchar>(i, j-1);
				uchar p9 = im.at<uchar>(i-1, j-1);

				/*uchar hash = createByte(p);*/
				uchar hash = p2 * 1 +
					p3 * 2 +
					p4 * 4 +
					p5 * 8 +
					p6 * 16 +
					p7 * 32 +
					p8 * 64 +
					p9 * 128;
				marker.at<uchar>(i,j) = table[iter][hash];

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

	createTable(0);
	createTable(1);

	//writeTable(0);
	
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
