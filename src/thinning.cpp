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

void GuoHallGenerateMaskTable(uchar* maskTable)
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
        bool b0 = C == 1;
        int b = (N >= 2 && N <= 3);
        maskTable[i] = (b0 && b & (((p6 | p7 | !p9) & p8) == 0)) ? 1 : 0;
        if (b0 && b & (((p2 | p3 | !p5) & p4) == 0))
            maskTable[i] |= (uchar)2;
    }
}

static int GuoHallIteration_optimized(cv::Mat& im,
                                      const uchar* maskTable,
                                      char m1,
                                      char m2)
{
    cv::Mat marker = cv::Mat::zeros(im.size(), CV_8UC1);
    int dif = 0;

    for (int i = 1; i < im.rows - 1; ++i)
    {
        for (int j = 1; j < im.cols - 1; ++j)
        {
            if (im.at<uchar>(i, j))
            {
                uchar p8 = im.at<uchar>(i, j-1);
                uchar p4 = im.at<uchar>(i, j+1);
                uchar p9 = im.at<uchar>(i-1, j-1);
                uchar p2 = im.at<uchar>(i-1, j);
                uchar p3 = im.at<uchar>(i-1, j+1);
                uchar p7 = im.at<uchar>(i+1, j-1);
                uchar p6 = im.at<uchar>(i+1, j);
                uchar p5 = im.at<uchar>(i+1, j+1);

                const int index =
                    (int)p2 * 1 +
                    (int)p3 * 2 +
                    (int)p4 * 4 +
                    (int)p5 * 8 +
                    (int)p6 * 16 +
                    (int)p7 * 32 +
                    (int)p8 * 64 +
                    (int)p9 * 128;

                if (maskTable[index] == m1 || maskTable[index] == m2)
                {
                    marker.at<uchar>(i, j) = 1;
                    dif = 1;
                }
            }
        }
    }

    if (dif > 0)
        im &= ~marker;
    return dif;
}

void GuoHallThinning_optimized(const cv::Mat& src, cv::Mat& dst)
{
    CV_Assert(CV_8UC1 == src.type());

    dst = src / 255;

    uchar maskTable[256];
    GuoHallGenerateMaskTable(maskTable);
    int dif = 0;
    do
    {
        dif = GuoHallIteration_optimized(dst, maskTable, 1, 3);
        dif += GuoHallIteration_optimized(dst, maskTable, 2, 3);
    }
    while (dif > 0);

    dst *= 255;
}


// Geometric mean

//          Name of Test

//                                   perf        perf        perf        perf        perf       perf       perf
//                                    res         res         res         res        res        res        res
//                                     0           1           2           3          1          2          3
//                                               luts         fix         fix        luts       fix        fix
//                                                and          2        copying      and         2       copying
//                                               zeros       luts                   zeros       luts        vs
//                                                                                    vs         vs

//                                                                                                         perf
//                                                                                   perf       perf       res
//                                                                                   res        res         0
//                                                                                    0          0      (x-factor)
//                                                                                (x-factor) (x-factor)
// Thinning::Size_Only::640x480   928.535 ms  549.163 ms  481.489 ms  464.392 ms     1.69       1.93       2.00
// Thinning::Size_Only::1280x720  2304.949 ms 1410.921 ms 1284.146 ms 1254.155 ms    1.63       1.79       1.84
// Thinning::Size_Only::1920x1080 6655.387 ms 4048.845 ms 3577.448 ms 3386.069 ms    1.64       1.86       1.97
