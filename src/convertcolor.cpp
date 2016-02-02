#include "skeleton_filter.hpp"

void ConvertColor_BGR2GRAY_BT709(const cv::Mat& src, cv::Mat& dst)
{
    CV_Assert(CV_8UC3 == src.type());
    cv::Size sz = src.size();
    dst.create(sz, CV_8UC1);

    const int bidx = 0;

    for (int y = 0; y < sz.height; ++y)
    {
        const cv::Vec3b *psrc = src.ptr<cv::Vec3b>(y);
        uchar *pdst = dst.ptr<uchar>(y);

        for (int x = 0; x < sz.width; ++x)
        {
            float color = 0.2126f * psrc[x][2-bidx] + 0.7152f * psrc[x][1] + 0.0722f * psrc[x][bidx];
            pdst[x] = (uchar)(color + 0.5);
        }
    }
}
