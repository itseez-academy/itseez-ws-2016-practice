#include "skeleton_filter.hpp"
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
                             ((x1 == x2) ? (int)(q11 * (y2 - y) + q22 * (y - y1)) :
                             ((y1 == y2) ? (int)(q11 * (x2 - x) + q22 * (x - x1)) : 
                              (int)(q11 * (x2 - x) * (y2 - y) + q21 * (x - x1) * (y2 - y) + q12 * (x2 - x) * (y - y1) + q22 * (x - x1) * (y - y1))));
            ptr_dst[col] = (temp < 0) ? 0 : ((temp > 255) ? 255 : (uchar)temp);
        }
    }
}

void ImageResize_optimized(const cv::Mat &src, cv::Mat &dst, const cv::Size sz)
{
    CV_Assert(CV_8UC1 == src.type());
    cv::Size sz_src = src.size();
    dst.create(sz, CV_8UC1);
	
	const float width = (float)sz_src.width  / sz.width;
	const float height = (float)sz_src.height / sz.height;
	const float width_coeff = .5f * width - .5f;
	const float height_coeff = .5f * height - .5f;

    for (int row = 0; row < sz.height; row++)
    {
        for (int col = 0; col < sz.width; col++)
        {
			const float x = col * width + width_coeff;
			const float y = row * height + height_coeff;

            const int ix = (int)(x);
            const int iy = (int)(y);

			int x1 = (ix < 0) ? 0 : ix, x2 = x1 + 1;
            int y1 = (iy < 0) ? 0 : iy, y2 = y1 + 1;

            const uchar q11 = src.data[src.step.p[0] * y1 + x1];
			const uchar q21 = src.data[src.step.p[0] * y1 + x2];
            const uchar q12 = src.data[src.step.p[0] * y2 + x1];
            const uchar q22 = src.data[src.step.p[0] * y2 + x2];

			const float y2y = (y2 - y);
			const float x2x = (x2 - x);
			const float yy1 = (y - y1);
			const float xx1 = (x - x1);

			const int temp = ((x1 == x2) && (y1 == y2)) ? (int)q11 :
                             ((x1 == x2) ? (int)(q11 * y2y + q22 * yy1) :
                             ((y1 == y2) ? (int)(q11 * x2x + q22 * xx1) : 
                              (int)(q11 * x2x * y2y + y2y * xx1 * q21 + q12 * x2x * yy1 + yy1 * q22 * xx1)));

           dst.data[dst.step.p[0] * row + col] = (uchar)temp;
        }
    }
}
