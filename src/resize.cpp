#include "skeleton_filter.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <math.h>

void ImageResize(const cv::Mat &src, cv::Mat &dst, const cv::Size sz)
{
    CV_Assert(CV_8UC1 == src.type());
    cv::Size sz_src = src.size();
    dst.create(sz, src.type());

    const int src_rows = src.rows;
    const int src_cols = src.cols;

    const int dst_rows = sz.height;
    const int dst_cols = sz.width;

    cv::Mat expandedSrc;
    cv::copyMakeBorder(src, expandedSrc, 0, 1, 0, 1, 1, 0);

    for (int row = 0; row < dst_rows; row++)
    {
        uchar *ptr_dst = dst.ptr<uchar>(row);

        for (int col = 0; col < dst_cols; col++)
        {
            const float x = ((float)col + .5f) * sz_src.width  / sz.width  - .5f;
            const float y = ((float)row + .5f) * sz_src.height / sz.height - .5f;

            const int ix = (int)floor(x);
            const int iy = (int)floor(y);            

            const int ix1 = (ix < 0) ? 0 : ((ix >= src_cols) ? src_cols - 1 : ix); //clamp[0, src_cols-1]
            const int ix2 = (ix < 0) ? 0 : ((ix > src_cols) ? src_cols : ix + 1);
            const int iy1 = (iy < 0) ? 0 : ((iy >= src_rows) ? src_rows - 1 : iy);//clamp[0, src_rows-1]
            const int iy2 = (iy < 0) ? 0 : ((iy > src_rows) ? src_rows : iy + 1);


            const uchar q11 = expandedSrc.at<uchar>(iy1, ix1);
            const uchar q12 = expandedSrc.at<uchar>(iy2, ix1);
            const uchar q21 = expandedSrc.at<uchar>(iy1, ix2);
            const uchar q22 = expandedSrc.at<uchar>(iy2, ix2);

            const int temp = (ix2 == src_cols) ? (int)(q11 * (iy2 - y) + q12 * (y - iy1)) :
                            ((iy2 == src_rows) ? (int)(q11 * (ix2 - x) + q21 * (x - ix1)) :
                                          (int)(q11 * (ix2 - x) * (iy2 - y) +
                                                q21 * (x - ix1) * (iy2 - y) +
                                                q12 * (ix2 - x) * (y - iy1) +
                                                q22 * (x - ix1) * (y - iy1)));

            ptr_dst[col] = (temp < 0) ? 0 : ((temp > 255) ? 255 : (uchar)temp);
        }
    }
}
