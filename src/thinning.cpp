#include "skeleton_filter.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <deque>

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

static void createTabs(cv::Vec<bool, 256>& tab0, cv::Vec<bool, 256>& tab1)
{
    uchar p2, p3, p4, p5, p6, p7, p8, p9;
    for(int code = 0; code < 256; code++)
    {
        p2 = code & 1;
        p3 = (code & 2) >> 1;
        p4 = (code & 4) >> 2;
        p5 = (code & 8) >> 3;
        p6 = (code & 16) >> 4;
        p7 = (code & 32) >> 5;
        p8 = (code & 64) >> 6;
        p9 = (code & 128) >> 7;

        int C  = (!p2 & (p3 | p4)) + (!p4 & (p5 | p6)) +
                    (!p6 & (p7 | p8)) + (!p8 & (p9 | p2));
        int N1 = (p9 | p2) + (p3 | p4) + (p5 | p6) + (p7 | p8);
        int N2 = (p2 | p3) + (p4 | p5) + (p6 | p7) + (p8 | p9);
        int N  = N1 < N2 ? N1 : N2;

        int m  = ((p6 | p7 | !p9) & p8);
        if (C == 1 && (N >= 2 && N <= 3) & (m == 0))
            tab0[code] = 1;

            m  = ((p2 | p3 | !p5) & p4);
        if (C == 1 && (N >= 2 && N <= 3) & (m == 0))
            tab1[code] = 1;
    }
}

static void GuoHall_stackInit(cv::Mat& im, cv::Mat& marker, 
                              cv::Vector<cv::Point2i>& stack, const cv::Vec<bool, 256>& tab0, const cv::Vec<bool, 256>& tab1)
{
    for (int i = 1; i < im.rows-1; i++)
    {
        for (int j = 1; j < im.cols-1; j++)
        {
            if(im.at<uchar>(i, j) == 0)
                continue;

            uchar p2 = im.at<uchar>(i-1, j);
            uchar p3 = im.at<uchar>(i-1, j+1);
            uchar p4 = im.at<uchar>(i, j+1);
            uchar p5 = im.at<uchar>(i+1, j+1);
            uchar p6 = im.at<uchar>(i+1, j);
            uchar p7 = im.at<uchar>(i+1, j-1);
            uchar p8 = im.at<uchar>(i, j-1);
            uchar p9 = im.at<uchar>(i-1, j-1);

            int code = p2 * 1 +
                       p3 * 2 +
                       p4 * 4 +
                       p5 * 8 +
                       p6 * 16 +
                       p7 * 32 +
                       p8 * 64 +
                       p9 * 128;

            if (tab0[code] || tab1[code])
            {
                im.at<uchar>(i, j) = 0;
                marker.at<uchar>(i,j) = 0;

                marker.at<uchar>(i-1, j) = 1;
                marker.at<uchar>(i-1, j+1) = 1;
                marker.at<uchar>(i, j+1) = 1;
                marker.at<uchar>(i+1, j+1) = 1;
                marker.at<uchar>(i+1, j) = 1;
                marker.at<uchar>(i+1, j-1) = 1;
                marker.at<uchar>(i, j-1) = 1;
                marker.at<uchar>(i-1, j-1) = 1;
            }
        }
    }

    for (int i = 1; i < im.rows-1; i++)
        for (int j = 1; j < im.cols-1; j++)
            if(marker.at<uchar>(i,j) == 1)
                stack.push_back(cv::Point2i(i, j));
}

void GuoHallThinning_optimized2(const cv::Mat& src, cv::Mat& dst)
{
    CV_Assert(CV_8UC1 == src.type());

    dst = src / 255;

    cv::Vec<bool, 256> tab0;
    cv::Vec<bool, 256> tab1;
    createTabs(tab0, tab1);

    cv::Mat marker = cv::Mat::zeros(dst.size(), CV_8UC1);
    cv::Vector<cv::Point2i> stack;

    GuoHall_stackInit(dst, marker, stack, tab0, tab1);
    while(!stack.empty())
    {
        int i = stack.back().x; 
        int j = stack.back().y; 
        stack.pop_back();
        marker.at<uchar>(i, j) = 0;

        if(dst.at<uchar>(i, j) == 0 || i == 0 || j == 0 || i == (dst.rows - 1) || j == (dst.cols - 1))
            continue;

        uchar p2 = dst.at<uchar>(i-1, j);
        uchar p3 = dst.at<uchar>(i-1, j+1);
        uchar p4 = dst.at<uchar>(i, j+1);
        uchar p5 = dst.at<uchar>(i+1, j+1);
        uchar p6 = dst.at<uchar>(i+1, j);
        uchar p7 = dst.at<uchar>(i+1, j-1);
        uchar p8 = dst.at<uchar>(i, j-1);
        uchar p9 = dst.at<uchar>(i-1, j-1);

        int code = p2 * 1 +
                       p3 * 2 +
                       p4 * 4 +
                       p5 * 8 +
                       p6 * 16 +
                       p7 * 32 +
                       p8 * 64 +
                       p9 * 128;

        if (tab0[code] || tab1[code])
        {
            dst.at<uchar>(i, j) = 0;
            marker.at<uchar>(i, j) = 0;

            if(marker.at<uchar>(i-1, j) == 0)
                stack.push_back(cv::Point2i(i-1, j));
            marker.at<uchar>(i-1, j) = 1;

            if(marker.at<uchar>(i-1, j+1) == 0)
                stack.push_back(cv::Point2i(i-1, j+1));
            marker.at<uchar>(i-1, j+1) = 1;

            if(marker.at<uchar>(i, j+1) == 0)
                stack.push_back(cv::Point2i(i, j+1));
            marker.at<uchar>(i, j+1) = 1;

            if(marker.at<uchar>(i+1, j+1) == 0)
                stack.push_back(cv::Point2i(i+1, j+1));
            marker.at<uchar>(i+1, j+1) = 1;

            if(marker.at<uchar>(i+1, j) == 0)
                stack.push_back(cv::Point2i(i+1, j));
            marker.at<uchar>(i+1, j) = 1;

            if(marker.at<uchar>(i+1, j-1) == 0)
                stack.push_back(cv::Point2i(i+1, j-1));
            marker.at<uchar>(i+1, j-1) = 1;

            if(marker.at<uchar>(i, j-1) == 0)
                stack.push_back(cv::Point2i(i, j-1));
            marker.at<uchar>(i, j-1) = 1;

            if(marker.at<uchar>(i-1, j-1) == 0)
                stack.push_back(cv::Point2i(i-1, j-1));
            marker.at<uchar>(i-1, j-1) = 1;
        }
    }

    dst *= 255;
}

static void GuoHallIteration_optimized(cv::Mat& im, const cv::Vec<bool, 256>& tab)
{
    cv::Mat marker = cv::Mat::zeros(im.size(), CV_8UC1);

    for (int i = 1; i < im.rows-1; i++)
    {
        for (int j = 1; j < im.cols-1; j++)
        {
            if(im.at<uchar>(i, j) == 0)
                continue;

            uchar p2 = im.at<uchar>(i-1, j);
            uchar p3 = im.at<uchar>(i-1, j+1);
            uchar p4 = im.at<uchar>(i, j+1);
            uchar p5 = im.at<uchar>(i+1, j+1);
            uchar p6 = im.at<uchar>(i+1, j);
            uchar p7 = im.at<uchar>(i+1, j-1);
            uchar p8 = im.at<uchar>(i, j-1);
            uchar p9 = im.at<uchar>(i-1, j-1);

            int code = p2 * 1 +
                       p3 * 2 +
                       p4 * 4 +
                       p5 * 8 +
                       p6 * 16 +
                       p7 * 32 +
                       p8 * 64 +
                       p9 * 128;

            if (tab[code])
                marker.at<uchar>(i,j) = 1;
        }
    }

    im &= ~marker;
}

void GuoHallThinning_optimized(const cv::Mat& src, cv::Mat& dst)
{
    CV_Assert(CV_8UC1 == src.type());

    dst = src / 255;

    cv::Vec<bool, 256> tab0;
    cv::Vec<bool, 256> tab1;
    createTabs(tab0, tab1);

    cv::Mat prev = cv::Mat::zeros(src.size(), CV_8UC1);
    cv::Mat diff;

    do
    {
        GuoHallIteration_optimized(dst, tab0);
        GuoHallIteration_optimized(dst, tab1);
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
