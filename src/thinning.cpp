#include "skeleton_filter.hpp"
#include <opencv2/imgproc/imgproc.hpp>

#include <vector>

static void GuoHallIteration(cv::Mat& im, int iter)
{
    cv::Mat marker = cv::Mat::zeros(im.size(), CV_8UC1);

    const uchar* row_up = im.ptr<uchar>(0);
    const uchar* row_middle = im.ptr<uchar>(1);
    const uchar* row_down = 0;
    uchar* marker_row = 0;
    for (int i = 1; i < im.rows-1; i++)
    {
        marker_row = marker.ptr<uchar>(i);
        row_down = im.ptr<uchar>(i + 1);
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
            {
                marker.at<uchar>(i,j) = 1;
            }
        }
    }

    im &= ~marker;
}

static void MakeTable(std::vector<unsigned>& table_iter0,
                      std::vector<unsigned>& table_iter1)
{
    table_iter0.resize(256);
    table_iter1.resize(256);
    unsigned value = 0;
    for (int i = 0; i < 256; ++i)
    {
        uchar p2 = i & 1;
        uchar p3 = i & 2;
        uchar p4 = i & 4;
        uchar p5 = i & 8;
        uchar p6 = i & 16;
        uchar p7 = i & 32;
        uchar p8 = i & 64;
        uchar p9 = i & 128;

        int C  = (!p2 & (p3 | p4)) + (!p4 & (p5 | p6)) +
                 (!p6 & (p7 | p8)) + (!p8 & (p9 | p2));
        int N1 = (p9 | p2) + (p3 | p4) + (p5 | p6) + (p7 | p8);
        int N2 = (p2 | p3) + (p4 | p5) + (p6 | p7) + (p8 | p9);
        int N  = N1 < N2 ? N1 : N2;
        if (C == 1 && (N >= 2 && N <= 3))
        {
            table_iter0[i] = !((p6 | p7 | !p9) & p8);
            table_iter1[i] = !((p2 | p3 | !p5) & p4);
        }
    }
}

void GuoHallThinning(const cv::Mat& src, cv::Mat& dst)
{
    CV_Assert(CV_8UC1 == src.type());

    dst = src / 255;

    cv::Mat prev = cv::Mat::zeros(src.size(), CV_8UC1);
    cv::Mat diff;

    std::vector<unsigned> table_iter0;
    std::vector<unsigned> table_iter1;
    MakeTable(table_iter0, table_iter1);

    do
    {
        GuoHallIteration(dst, 0, table_iter0, table_iter1);
        GuoHallIteration(dst, 1, table_iter0, table_iter1);
        cv::absdiff(dst, prev, diff);
        dst.copyTo(prev);
    }
    while (cv::countNonZero(diff) > 0);

    dst *= 255;
}

//
// Place optimized version here
//

static void MakeTables(std::vector<uchar>& table_iter0,
                       std::vector<uchar>& table_iter1)
{
    table_iter0.clear();
    table_iter1.clear();
    table_iter0.resize(256, 0);
    table_iter1.resize(256, 0);
    for (int i = 0; i < 256; ++i)
    {
        uchar p2 = i & 1;
        uchar p3 = (i & 2) >> 1;
        uchar p4 = (i & 4) >> 2;
        uchar p5 = (i & 8) >> 3;
        uchar p6 = (i & 16) >> 4;
        uchar p7 = (i & 32) >> 5;
        uchar p8 = (i & 64) >> 6;
        uchar p9 = (i & 128) >> 7;

        int C  = (!p2 & (p3 | p4)) + (!p4 & (p5 | p6)) +
                 (!p6 & (p7 | p8)) + (!p8 & (p9 | p2));
        int N1 = (p9 | p2) + (p3 | p4) + (p5 | p6) + (p7 | p8);
        int N2 = (p2 | p3) + (p4 | p5) + (p6 | p7) + (p8 | p9);
        int N  = N1 < N2 ? N1 : N2;
        if (C == 1 && (N >= 2 && N <= 3))
        {
            table_iter0[i] = !((p6 | p7 | !p9) & p8);
            table_iter1[i] = !((p2 | p3 | !p5) & p4);
        }
    }
}

static void GuoHallIteration_optimized(cv::Mat& im, int iter,
                                       const std::vector<uchar>& table_iter0,
                                       const std::vector<uchar>& table_iter1)
{
    cv::Mat marker = cv::Mat::zeros(im.size(), CV_8UC1);

     const uchar* row_up = im.ptr<uchar>(0);
     const uchar* row_middle = im.ptr<uchar>(1);
     const uchar* row_down = 0;
     uchar* marker_row = 0;
    for (int i = 1; i < im.rows-1; i++)
    {
        marker_row = marker.ptr<uchar>(i);
        row_down = im.ptr<uchar>(i + 1);
        for (int j = 1; j < im.cols-1; j++)
        {
            if (row_middle[j] != 0)
            {
                unsigned code = row_up[j] +
                                2 * row_up[j + 1] +
                                4 * row_middle[j + 1] +
                                8 * row_down[j + 1] +
                                16 * row_down[j] +
                                32 * row_down[j - 1] +
                                64 * row_middle[j - 1] +
                                128 * row_up[j - 1];
                if (iter != 0)
                {
                    if (table_iter1[code])
                        marker_row[j] = table_iter1[code];
                }
                else
                {
                    if (table_iter0[code])
                        marker_row[j] = table_iter0[code];
                }                    
            }
        }
        row_up = row_middle;
        row_middle = row_down;
    }

    im &= ~marker;
}

void GuoHallThinning_optimized(const cv::Mat& src, cv::Mat& dst)
{
    CV_Assert(CV_8UC1 == src.type());

    dst = src / 255;

    cv::Mat prev = cv::Mat::zeros(src.size(), CV_8UC1);
    cv::Mat diff;

    std::vector<uchar> table_iter0;
    std::vector<uchar> table_iter1;
    MakeTables(table_iter0, table_iter1);

    do
    {
        GuoHallIteration_optimized(dst, 0, table_iter0, table_iter1);
        GuoHallIteration_optimized(dst, 1, table_iter0, table_iter1);
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
