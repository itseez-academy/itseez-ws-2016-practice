#include "skeleton_filter.hpp"
#include <math.h>
using namespace std;
void ImageResize(const cv::Mat &src, cv::Mat &dst, const cv::Size sz)
{
    CV_Assert(CV_8UC1 == src.type());
    cv::Size sz_src = src.size();
    dst.create(sz, src.type());

    const int src_rows = src.rows;
    const int src_cols = src.cols;

    const int dst_rows = sz.height;
    const int dst_cols = sz.width;

    for (int row = 0; row < dst_rows; row++)
    {
        uchar *ptr_dst = dst.ptr<uchar>(row);

        for (int col = 0; col < dst_cols; col++)
        {
            const float x = ((float)col + .5f) * sz_src.width  / sz.width  - .5f;
            const float y = ((float)row + .5f) * sz_src.height / sz.height - .5f;

            const int ix = (int)floor(x);
            const int iy = (int)floor(y);

            const int x1 = (ix < 0) ? 0 : ((ix >= src_cols) ? src_cols - 1 : ix);
            const int x2 = (ix < 0) ? 0 : ((ix >= src_cols - 1) ? src_cols - 1 : ix + 1);
            const int y1 = (iy < 0) ? 0 : ((iy >= src_rows) ? src_rows - 1 : iy);
            const int y2 = (iy < 0) ? 0 : ((iy >= src_rows - 1) ? src_rows - 1 : iy + 1);

            const uchar q11 = src.at<uchar>(y1, x1);
            const uchar q12 = src.at<uchar>(y2, x1);
            const uchar q21 = src.at<uchar>(y1, x2);
            const uchar q22 = src.at<uchar>(y2, x2);

            const int temp = ((x1 == x2) && (y1 == y2)) ? (int)q11 :
                             ( (x1 == x2) ? (int)(q11 * (y2 - y) + q22 * (y - y1)) :
                               ( (y1 == y2) ? (int)(q11 * (x2 - x) + q22 * (x - x1)) : 
                                 (int)(q11 * (x2 - x) * (y2 - y) + q21 * (x - x1) * (y2 - y) + q12 * (x2 - x) * (y - y1) + q22 * (x - x1) * (y - y1))));
            ptr_dst[col] = (temp < 0) ? 0 : ((temp > 255) ? 255 : (uchar)temp);
        }
    }
}

void ImageResize_optimized(const cv::Mat &src, cv::Mat &dst, const cv::Size sz)
{
    CV_Assert(CV_8UC1 == src.type());
    cv::Size sz_src = src.size();
    dst.create(sz, src.type());

    const int src_rows = src.rows;
    const int src_cols = src.cols;

    const int dst_rows = sz.height;
    const int dst_cols = sz.width;
    float y_scale = (float)sz_src.height / (float)sz.height;
    float x_scale = (float)sz_src.width / (float)sz.width;
    float y_0 = .5f * y_scale - .5f ;
    float x_0 = .5f * x_scale - .5f ;
    for (int row = 0; row < dst_rows; row++)
    {
        uchar *ptr_dst = dst.ptr<uchar>(row);

        const float y = (float)row * y_scale + y_0;
        int iy = (int)y;
        int right_bound_check_y1 = (int)(min(src_rows, iy));
        int right_bound_check_y2 = (int)min(src_rows - 1, iy + 1);
            
        int y1 = (int)(max(0, right_bound_check_y1));
        int y2 = (int)(max(0, right_bound_check_y2));
        const uchar *y1_row = src.ptr<uchar>(y1);
        const uchar *y2_row = src.ptr<uchar>(y2);

        for (int col = 0; col < dst_cols; col++)
        {
            const float x = (float)col * x_scale + x_0;

            int ix = (int)x;
            int right_bound_check_x1 = (int)min(src_cols, ix);
            int right_bound_check_x2 = (int)min(src_cols - 1, ix + 1);
            int x1 = (int)max(0, right_bound_check_x1);
            int x2 = (int)max(0, right_bound_check_x2);

            uchar q11 = y1_row[x1];
            uchar q21 = y1_row[x2];
            
            uchar q12 = y2_row[x1];
            uchar q22 = y2_row[x2];

            const int temp = ((x1 == x2) && (y1 == y2)) ? (int)q11 :
              ( (x1 == x2) ? (int)(q11 * (y2 - y) + q22 * (y - y1)) :
              ( (y1 == y2) ? (int)(q11 * (x2 - x) + q22 * (x - x1)) : 
              (int)(q11 * (x2 - x) * (y2 - y) + q21 * (x - x1) * (y2 - y) + q12 * (x2 - x) * (y - y1) + q22 * (x - x1) * (y - y1))));
            ptr_dst[col] = (uchar)temp;
        }
    }
}
