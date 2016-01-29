#ifndef _INTERNAL_HPP_
#define _INTERNAL_HPP_

#include "opencv2/imgproc/imgproc.hpp"

void ImageResize(const cv::Mat &src, cv::Mat &dst, const cv::Size sz);
void GuoHallThinning(const cv::Mat& src, cv::Mat& dst);
void ConvertColor_BGR2GRAY_BT709(const cv::Mat& src, cv::Mat& dst);

#endif
