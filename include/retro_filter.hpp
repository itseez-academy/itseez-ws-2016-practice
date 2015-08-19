#ifndef __RETRO_FILTER_H__
#define __RETRO_FILTER_H__

#include "opencv2/core/core.hpp"

class RetroFilter
{
 public:
    struct Parameters
    {
        cv::Size frameSize;
        cv::Mat fuzzyBorder;
        cv::Mat scratches;
    };

    RetroFilter(const Parameters& params);
    virtual ~RetroFilter() {};

    void applyToVideo(const cv::Mat& frame, cv::Mat& retroFrame);

 private:
    Parameters params_;
    cv::RNG rng_;

    float hsvScale_;
    float hsvOffset_;
};

#endif //__RETRO_FILTER_H__
