#pragma once

#include "opencv2/core/core.hpp"

// Pipeline
void skeletonize(const cv::Mat& input, cv::Mat& output, bool save_images);

// Internal functions
void ImageResize(const cv::Mat &src, cv::Mat &dst, const cv::Size sz);
void GuoHallThinning(const cv::Mat& src, cv::Mat& dst);
void ConvertColor_BGR2GRAY_BT709(const cv::Mat& src, cv::Mat& dst);
