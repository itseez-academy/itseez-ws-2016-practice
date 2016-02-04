#include "skeleton_filter.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
using namespace std;
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

static void Decode(uchar* p, uchar code)
{
    p[0] = code & 1;//p2
    p[1] = (code & 2) >> 1;//p3
    p[2] = (code & 4) >> 2;//p4
    p[3] = (code & 8) >> 3;//p5
    p[4] = (code & 16) >> 4;//p6
    p[5] = (code & 32) >> 5;//p7
    p[6] = (code & 64) >> 6;//p8
    p[7] = (code & 128) >> 7;//p9
}

static uchar Encode(uchar* p)
{
    uchar code = p[0] * 1 + p[1] * 2 + p[2] * 4 +
       p[3] * 8 + p[4] * 16 + p[5] * 32 + p[6] * 64 + p[7] * 128;
    return code;
}



static void calculateTables(uchar* neighbours_to_value_zero, uchar* neighbours_to_value_one)
{
    uchar p[8];
    uchar  C, N1, N2, N, m_zero, m_one;
    for(uchar code = 0; code < 255; code++)
    {
        Decode(p, code);
        C  = (!p[0] & (p[1] | p[2])) + (!p[2] & (p[3] | p[4])) +
                     (!p[4] & (p[5] | p[6])) + (!p[6] & (p[7] | p[0]));
        N1 = (p[7] | p[0]) + (p[1] | p[2]) + (p[3] | p[4]) + (p[5] | p[6]);
        N2 = (p[0] | p[1]) + (p[2] | p[3]) + (p[4] | p[5]) + (p[6] | p[7]);
        N  = N1 < N2 ? N1 : N2;
        m_zero  = ((p[4] | p[5] | !p[7]) & p[6]);
        m_one = ((p[0] | p[1] | !p[3]) & p[2]);
        if (C == 1 && (N >= 2 && N <= 3) & (m_zero == 0))
        {
            neighbours_to_value_zero[code] = 1;
        }
        else
        {
            neighbours_to_value_zero[code] = 0;
        }
        if (C == 1 && (N >= 2 && N <= 3) & (m_one == 0))
        {
            neighbours_to_value_one[code] = 1;
        }
        else
        {
            neighbours_to_value_one[code] = 0;
        }
    }
}

static void GuoHallIteration_optimized(cv::Mat& im, uchar* neighbours_to_value)
{
    cv::Mat marker = cv::Mat::zeros(im.size(), CV_8UC1);
    uchar p[8] = {0};
    uchar code = 0;
    for (int i = 1; i < im.rows-1; i++)
    {
        uchar* prev_row = im.ptr<uchar>(i-1);
        uchar* cur_row = im.ptr<uchar>(i);
        uchar* next_row = im.ptr<uchar>(i+1);
        for (int j = 1; j < im.cols-1; j++)
        {
            if(cur_row[j] > 0)
            {
                p[6] = cur_row[j-1];
                p[2] = cur_row[j+1];

                p[7] = prev_row[j-1];
                p[0] = prev_row[j];
                p[1] = prev_row[j+1];

                p[5] = next_row[j-1];
                p[3] = next_row[j+1];
                p[4] = next_row[j];
                
                code = Encode(p);
                marker.at<uchar>(i,j) = neighbours_to_value[code];
            }
        }
    }

    im &= ~marker;
}

void GuoHallThinning_optimized(const cv::Mat& src, cv::Mat& dst)
{
    CV_Assert(CV_8UC1 == src.type());
    dst = src / 255;
    uchar neighbours_to_value_one[256] = {0};
    uchar neighbours_to_value_zero[256] = {0};
    calculateTables(neighbours_to_value_zero, neighbours_to_value_one);
    uint prev_non_zeros = 0;
    uint current_non_zeros = cv::countNonZero(dst);
    do
    {
        prev_non_zeros = current_non_zeros;
        GuoHallIteration_optimized(dst, neighbours_to_value_zero);
        GuoHallIteration_optimized(dst, neighbours_to_value_one);
        current_non_zeros = cv::countNonZero(dst);
     }
    while(current_non_zeros < prev_non_zeros);
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
