#include "skeleton_filter.hpp"
#include <opencv2/imgproc/imgproc.hpp>

#include <vector>
#include <algorithm>
#include <utility>

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

static void decode(uchar area, uchar* p2, uchar* p3, uchar* p4,
    uchar* p5, uchar* p6, uchar* p7,
    uchar* p8, uchar* p9)
{
    *p2 = (bool) (area &   1);
    *p3 = (bool) (area &   2);
    *p4 = (bool) (area &   4);
    *p5 = (bool) (area &   8);
    *p6 = (bool) (area &  16);
    *p7 = (bool) (area &  32);
    *p8 = (bool) (area &  64);
    *p9 = (bool) (area & 128);
}

static int encode(uchar p2, uchar p3, uchar p4,
    uchar p5, uchar p6, uchar p7,
    uchar p8, uchar p9)
{
    return
       p2 *   1 +
       p3 *   2 +
       p4 *   4 +
       p5 *   8 +
       p6 *  16 +
       p7 *  32 +
       p8 *  64 +
       p9 * 128;
}

static int GuoHallIteration_optimized(cv::Mat& dst, const cv::Mat& src, const std::vector<uchar>& table)
{
    int rows = src.rows;
    int cols = src.cols;

    int changed = 0;

    for (int row = 1; row < rows-1; row++)
    {
        const uchar* predRow = src.ptr<uchar>(row-1);
        const uchar* thisRow = src.ptr<uchar>(row-0);
        const uchar* nextRow = src.ptr<uchar>(row+1);

        uchar* destRow = dst.ptr<uchar>(row);

        for (int col = 1; col < cols-1; col++)
        {
            if (thisRow[col] == 0)
            {
                destRow[col] = 0;
                continue;
            }

            uchar p9 = predRow[col-1];
            uchar p2 = predRow[col+0];
            uchar p3 = predRow[col+1];

            uchar p8 = thisRow[col-1];
            uchar p4 = thisRow[col+1];

            uchar p7 = nextRow[col-1];
            uchar p6 = nextRow[col+0];
            uchar p5 = nextRow[col+1];

            int code = encode(p2, p3 ,p4, p5, p6, p7, p8, p9);
            changed |= (destRow[col] xor table[code]);
            destRow[col] = table[code];
        }
    }

    return changed;
}

static std::vector<uchar> fillTable(int iter)
{
    const int nVariants = 256;
    std::vector<uchar> table(256, 1);
    for (int variant = 0; variant < nVariants; variant++)
    {
        uchar p2, p3, p4, p5, p6, p7, p8, p9;
        decode(variant, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9);

        int C  = (!p2 & (p3 | p4)) + (!p4 & (p5 | p6)) +
                 (!p6 & (p7 | p8)) + (!p8 & (p9 | p2));
        int N1 = (p9 | p2) + (p3 | p4) + (p5 | p6) + (p7 | p8);
        int N2 = (p2 | p3) + (p4 | p5) + (p6 | p7) + (p8 | p9);
        int N  = std::min(N1, N2);
        int m  = (1-iter)*((p6 | p7 | !p9) & p8)+iter*((p2 | p3 | !p5) & p4);

        if (C == 1 && (N >= 2 && N <= 3) & (m == 0))
        {
            table[variant] = 0;
        }
    }

    return table;
}

void GuoHallThinning_optimized(const cv::Mat& src, cv::Mat& dest)
{
    CV_Assert(CV_8UC1 == src.type());
    dest = src / 255;

    cv::Mat prev = dest.clone();
    const std::vector<uchar> table0 = fillTable(0);
    const std::vector<uchar> table1 = fillTable(1);
    int changed0 = 0;
    int changed1 = 0;

    do
    {
        changed0 = GuoHallIteration_optimized(prev, dest, table0);
        changed1 = GuoHallIteration_optimized(dest, prev, table1);
    }
    while (changed0 || changed1);

    dest *= 255;
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
