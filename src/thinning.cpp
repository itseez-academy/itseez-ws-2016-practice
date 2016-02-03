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

void GuoHallGenerateMaskTable(uchar* maskTable, int iter)
{
    for (int i = 0; i < 256; ++i)
    {
        const int 
            p2 = ((i & (int)1) >> 0),
            p3 = ((i & (int)2) >> 1),
            p4 = ((i & (int)4) >> 2),
            p5 = ((i & (int)8) >> 3),
            p6 = ((i & (int)16) >> 4),
            p7 = ((i & (int)32) >> 5),
            p8 = ((i & (int)64) >> 6),
            p9 = ((i & (int)128) >> 7);

        int C  = (!p2 & (p3 | p4)) + (!p4 & (p5 | p6)) +
            (!p6 & (p7 | p8)) + (!p8 & (p9 | p2));
        int N1 = (p9 | p2) + (p3 | p4) + (p5 | p6) + (p7 | p8);
        int N2 = (p2 | p3) + (p4 | p5) + (p6 | p7) + (p8 | p9);
        int N  = N1 < N2 ? N1 : N2;
        int m  = iter == 0 ? ((p6 | p7 | !p9) & p8) : ((p2 | p3 | !p5) & p4);

        maskTable[i] = (C == 1 && (N >= 2 && N <= 3) & (m == 0)) ? 1 : 0;
    }
}

static void GuoHallIteration_optimized(cv::Mat& im, const uchar* maskTable)
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

                const int index = 
                    (int)(p2 ? 1 : 0) * 1 +
                    (int)(p3 ? 1 : 0) * 2 +
                    (int)(p4 ? 1 : 0) * 4 +
                    (int)(p5 ? 1 : 0) * 8 +
                    (int)(p6 ? 1 : 0) * 16 +
                    (int)(p7 ? 1 : 0) * 32 +
                    (int)(p8 ? 1 : 0) * 64 +
                    (int)(p9 ? 1 : 0) * 128;
                
                marker.at<uchar>(i,j) |= maskTable[index];
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

    uchar maskTable0[256];
    uchar maskTable1[256];

    GuoHallGenerateMaskTable(maskTable0, 0);
    GuoHallGenerateMaskTable(maskTable1, 1);

    do
    {
        GuoHallIteration_optimized(dst, maskTable0);
        GuoHallIteration_optimized(dst, maskTable1);
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
