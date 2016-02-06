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

static void GuoHallAddToBuffer(int i,
                               int j,
                               uchar value,
                               int rows,
                               int cols,
                               int* buffer,
                               int &bufferCount,
                               uchar* table)
{
    if (value > 0 && i > 0 && i < rows - 1 && j > 0 && j < cols - 1)
    {
        int index = i * cols + j;
        if (table[index] == 0)
        {
            table[index] = 1;
            buffer[bufferCount++] = index;
        }
    }
}

static int GuoHallGetLutIndex(uchar p2, uchar p3, uchar p4, uchar p5,
                              uchar p6, uchar p7, uchar p8, uchar p9)
{
    return ((int)p2 * 1 +
            (int)p3 * 2 +
            (int)p4 * 4 +
            (int)p5 * 8 +
            (int)p6 * 16 +
            (int)p7 * 32 +
            (int)p8 * 64 +
            (int)p9 * 128) / 255;
}

static bool GuoHallIsChange(int index,
                            const uchar* maskTable,
                            char m1,
                            char m2)
{
    return maskTable[index] == m1 || maskTable[index] == m2;
}

static int GuoHallNext(int* &buffer,
                       int &bufferCount,
                       uchar* table,
                       size_t tableSize,
                       int* &nextBuffer,
                       int &nextBufferCount)
{
    memset(table, 0, tableSize);
    std::swap(buffer, nextBuffer);
    bufferCount = nextBufferCount;
    nextBufferCount = 0;
    return bufferCount > 0 ? 1 : 0;
}

static void GuoHallIteration1_optimized(cv::Mat& im,
                                        const uchar* maskTable,
                                        char m1,
                                        char m2,
                                        int* buffer,
                                        int &bufferCount,
                                        uchar* table)
{
    cv::Mat marker(im.size(), CV_8UC1, 255);
    for (int i = 1; i < im.rows - 1; ++i)
    {
        for (int j = 1; j < im.cols - 1; ++j)
        {
            if (im.at<uchar>(i, j) > 0)
            {
                uchar p8 = im.at<uchar>(i, j-1);
                uchar p4 = im.at<uchar>(i, j+1);
                uchar p9 = im.at<uchar>(i-1, j-1);
                uchar p2 = im.at<uchar>(i-1, j);
                uchar p3 = im.at<uchar>(i-1, j+1);
                uchar p7 = im.at<uchar>(i+1, j-1);
                uchar p6 = im.at<uchar>(i+1, j);
                uchar p5 = im.at<uchar>(i+1, j+1);

                const int index = GuoHallGetLutIndex(p2, p3, p4, p5,
                                                     p6, p7, p8, p9);
                if (GuoHallIsChange(index, maskTable, m1, m2))
                {
                    GuoHallAddToBuffer(i - 1, j - 1, p9, im.rows, im.cols,
                                       buffer, bufferCount, table);
                    GuoHallAddToBuffer(i - 1, j, p2, im.rows, im.cols,
                                       buffer, bufferCount, table);
                    GuoHallAddToBuffer(i - 1, j + 1, p3, im.rows, im.cols,
                                       buffer, bufferCount, table);
                    GuoHallAddToBuffer(i, j - 1, p8, im.rows, im.cols,
                                       buffer, bufferCount, table);
                    GuoHallAddToBuffer(i, j + 1, p4, im.rows, im.cols,
                                       buffer, bufferCount, table);
                    GuoHallAddToBuffer(i + 1, j - 1, p7, im.rows, im.cols,
                                       buffer, bufferCount, table);
                    GuoHallAddToBuffer(i + 1, j, p6, im.rows, im.cols,
                                       buffer, bufferCount, table);
                    GuoHallAddToBuffer(i + 1, j + 1, p5, im.rows, im.cols,
                                       buffer, bufferCount, table);
                    marker.at<uchar>(i, j) = 0;
                }
            }
        }
    }
    im &= marker;
}
static void GuoHallIteration2_optimized(cv::Mat& im,
                                        const uchar* maskTable,
                                        char m1,
                                        char m2,
                                        int* buffer,
                                        int bufferCount,
                                        uchar* table,
                                        int* nextBuffer,
                                        int &nextBufferCount)
{
    cv::Mat marker(im.size(), CV_8UC1, 255);
    for (int bi = 0; bi < bufferCount; ++bi)
    {
        //printf("%d\n", bi);
        const int bii = buffer[bi];
        //printf("%d\n", bii);

        const int i = bii / im.cols;
        const int j = bii % im.cols;
        uchar p8 = im.at<uchar>(i, j-1);
        uchar p4 = im.at<uchar>(i, j+1);
        uchar p9 = im.at<uchar>(i-1, j-1);
        uchar p2 = im.at<uchar>(i-1, j);
        uchar p3 = im.at<uchar>(i-1, j+1);
        uchar p7 = im.at<uchar>(i+1, j-1);
        uchar p6 = im.at<uchar>(i+1, j);
        uchar p5 = im.at<uchar>(i+1, j+1);
        const int index = GuoHallGetLutIndex(p2, p3, p4, p5,
                                             p6, p7, p8, p9);
        if (GuoHallIsChange(index, maskTable, m1, m2))
        {
            GuoHallAddToBuffer(i - 1, j - 1, p9, im.rows, im.cols,
                               nextBuffer, nextBufferCount, table);
            GuoHallAddToBuffer(i - 1, j, p2, im.rows, im.cols,
                               nextBuffer, nextBufferCount, table);
            GuoHallAddToBuffer(i - 1, j + 1, p3, im.rows, im.cols,
                               nextBuffer, nextBufferCount, table);
            GuoHallAddToBuffer(i, j - 1, p8, im.rows, im.cols,
                               nextBuffer, nextBufferCount, table);
            GuoHallAddToBuffer(i, j + 1, p4, im.rows, im.cols,
                               nextBuffer, nextBufferCount, table);
            GuoHallAddToBuffer(i + 1, j - 1, p7, im.rows, im.cols,
                               nextBuffer, nextBufferCount, table);
            GuoHallAddToBuffer(i + 1, j, p6, im.rows, im.cols,
                               nextBuffer, nextBufferCount, table);
            GuoHallAddToBuffer(i + 1, j + 1, p5, im.rows, im.cols,
                               nextBuffer, nextBufferCount, table);
            marker.at<uchar>(i, j) = 0;
        }
    }
    im &= marker;
}

void GuoHallThinning_optimized(const cv::Mat& src, cv::Mat& dst)
{
    CV_Assert(CV_8UC1 == src.type());

    dst = src.clone();

    uchar maskTable[256];
    GuoHallGenerateMaskTable(maskTable);
    const size_t tableSize = static_cast<size_t>(src.cols) *
                             static_cast<size_t>(src.rows);
    int* buffers = reinterpret_cast<int*>(malloc(tableSize * sizeof(int) * 2));
    int* buffer = buffers;
    int* nextBuffer = buffers + tableSize;
    int bufferCount = 0;
    int nextBufferCount = 0;
    uchar* table = reinterpret_cast<uchar*>(malloc(tableSize));
    memset(table, 0, tableSize);

    GuoHallIteration1_optimized(dst, maskTable, 1, 3, buffer, bufferCount,
                                table);

    int cnt = GuoHallNext(nextBuffer, nextBufferCount, table,
                          tableSize, buffer, bufferCount);

    //printf("%d, %d, %d\n", bufferCount, nextBufferCount, tableSize);
    //std::swap(buffer, nextBuffer);

    GuoHallIteration2_optimized(dst, maskTable, 2, 3, buffer, bufferCount,
                                table, nextBuffer, nextBufferCount);

    //printf("%d, %d\n", bufferCount, nextBufferCount);
    cnt += GuoHallNext(buffer, bufferCount, table,
                       tableSize, nextBuffer, nextBufferCount);


    while (cnt > 0)
    {
        //printf("%d, %d\n", bufferCount, nextBufferCount);
        GuoHallIteration2_optimized(dst, maskTable, 1, 3, buffer, bufferCount,
                                    table, nextBuffer, nextBufferCount);
        cnt = GuoHallNext(buffer, bufferCount, table,
                          tableSize, nextBuffer, nextBufferCount);
        GuoHallIteration2_optimized(dst, maskTable, 2, 3, buffer, bufferCount,
                                    table, nextBuffer, nextBufferCount);
        cnt += GuoHallNext(buffer, bufferCount, table,
                           tableSize, nextBuffer, nextBufferCount);
    }

    free(table);
    free(buffers);
}

static int GuoHallIteration_preoptimized(cv::Mat& im,
                                         const uchar* maskTable,
                                         char m1,
                                         char m2,
                                         uchar* top,
                                         uchar* topNext)
{
    int dif = 0;
    memcpy(top, im.data, im.cols);
    for (int i = 1; i < im.rows - 1; ++i)
    {
        memcpy(topNext, im.data + i * im.cols, im.cols);
        uchar left = im.at<uchar>(i, 0);
        for (int j = 1; j < im.cols - 1; ++j)
        {
            if (im.at<uchar>(i, j))
            {
                uchar p4 = im.at<uchar>(i, j+1);
                uchar p7 = im.at<uchar>(i+1, j-1);
                uchar p6 = im.at<uchar>(i+1, j);
                uchar p5 = im.at<uchar>(i+1, j+1);
                uchar p9 = top[j - 1];
                uchar p2 = top[j];
                uchar p3 = top[j + 1];
                uchar p8 = left;

                const int index =
                    ((int)p2 * 1 +
                    (int)p3 * 2 +
                    (int)p4 * 4 +
                    (int)p5 * 8 +
                    (int)p6 * 16 +
                    (int)p7 * 32 +
                    (int)p8 * 64 +
                    (int)p9 * 128) / 255;

                left = im.at<uchar>(i, j);
                if (maskTable[index] == m1 || maskTable[index] == m2)
                {
                    im.at<uchar>(i, j) = 0;
                    dif = 1;
                }
            }
            else
            {
                left = 0;
            }
        }
        std::swap(top, topNext);
    }
    return dif;
}

void GuoHallThinning_preoptimized(const cv::Mat& src, cv::Mat& dst)
{
    CV_Assert(CV_8UC1 == src.type());

    dst = src.clone();

    uchar* top = (uchar*)malloc(src.cols * 2);
    uchar* topNext = top + src.cols;
    uchar maskTable[256];
    GuoHallGenerateMaskTable(maskTable);
    int dif = 0;
    do
    {
        dif = GuoHallIteration_preoptimized(dst, maskTable, 1, 3,
                                            top, topNext);
        dif += GuoHallIteration_preoptimized(dst, maskTable, 2, 3,
                                             top, topNext);
    }
    while (dif > 0);
    free(top);
}

//Geometric mean

//         Name of Test

//                                  perf        perf       perf       perf       perf
//                                   res         res        res       res        res
//                                    0           1          1         1          1
//                                              first     realgo     first      realgo
//                                               opt                  opt         vs
//                                                                     vs

//                                                                               perf
//                                                                    perf       res
//                                                                    res         0
//                                                                     0      (x-factor)
//                                                                 (x-factor)
//Thinning::Size_Only::640x480   970.769 ms  355.407 ms  6.101 ms     2.73      159.11
//Thinning::Size_Only::1280x720  2421.474 ms 886.766 ms  19.683 ms    2.73      123.03
//Thinning::Size_Only::1920x1080 6995.375 ms 2477.897 ms 45.317 ms    2.82      154.37

