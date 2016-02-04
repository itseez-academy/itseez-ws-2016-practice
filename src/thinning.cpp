#include "skeleton_filter.hpp"
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

static void GuoHallIteration(cv::Mat &im, int iter) {
    cv::Mat marker = cv::Mat::zeros(im.size(), CV_8UC1);

    for (int i = 1; i < im.rows - 1; i++) {
        for (int j = 1; j < im.cols - 1; j++) {
            uchar p2 = im.at<uchar>(i - 1, j);
            uchar p3 = im.at<uchar>(i - 1, j + 1);
            uchar p4 = im.at<uchar>(i, j + 1);
            uchar p5 = im.at<uchar>(i + 1, j + 1);
            uchar p6 = im.at<uchar>(i + 1, j);
            uchar p7 = im.at<uchar>(i + 1, j - 1);
            uchar p8 = im.at<uchar>(i, j - 1);
            uchar p9 = im.at<uchar>(i - 1, j - 1);

            int C = (!p2 & (p3 | p4)) + (!p4 & (p5 | p6)) +
                    (!p6 & (p7 | p8)) + (!p8 & (p9 | p2));
            int N1 = (p9 | p2) + (p3 | p4) + (p5 | p6) + (p7 | p8);
            int N2 = (p2 | p3) + (p4 | p5) + (p6 | p7) + (p8 | p9);
            int N = N1 < N2 ? N1 : N2;
            int m = iter == 0 ? ((p6 | p7 | !p9) & p8) : ((p2 | p3 | !p5) & p4);

            if (C == 1 && (N >= 2 && N <= 3) & (m == 0))
                marker.at<uchar>(i, j) = 1;
        }
    }

    im &= ~marker;
}

void GuoHallThinning(const cv::Mat &src, cv::Mat &dst) {
    CV_Assert(CV_8UC1 == src.type());

    dst = src / 255;

    cv::Mat prev = cv::Mat::zeros(src.size(), CV_8UC1);
    cv::Mat diff;

    do {
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
namespace {

    uchar new_value_0[256];
    uchar new_value_1[256];

    struct Initializer {
        Initializer() {

            int guo_c[256];
            int guo_N1[256];
            int guo_N2[256];
            int guo_lm[256];
            int guo_rm[256];

            for (uchar i = 0; i <= uchar(254); ++i) {
                uchar p[8];
                uchar ti = i;
                for (int j = 7; j >= 0; --j) {
                    p[j] = ti & uchar(1);
                    ti >>= 1;
                }

                guo_c[i] = (!p[0] & (p[1] | p[2])) + (!p[2] & (p[3] | p[4])) +
                           (!p[4] & (p[5] | p[6])) + (!p[6] & (p[7] | p[0]));
                guo_N1[i] = (p[7] | p[0]) + (p[1] | p[2]) + (p[3] | p[4]) + (p[5] | p[6]);
                guo_N2[i] = (p[0] | p[1]) + (p[2] | p[3]) + (p[4] | p[5]) + (p[6] | p[7]);
                guo_lm[i] = (p[4] | p[5] | !p[7]) & p[6];
                guo_rm[i] = (p[0] | p[1] | !p[3]) & p[2];


                int N = guo_N1[i] < guo_N2[i] ? guo_N1[i] : guo_N2[i];
                int m_0 = guo_lm[i];
                int m_1 = guo_rm[i];

                if (guo_c[i] == 1 && (N >= 2 && N <= 3) & (m_0 == 0))
                    new_value_0[i] = 1;
                else
                    new_value_0[i] = 0;

                if (guo_c[i] == 1 && (N >= 2 && N <= 3) & (m_1 == 0))
                    new_value_1[i] = 1;
                else
                    new_value_1[i] = 0;
            }
            uchar i = 255;
            uchar p[8];
            uchar ti = i;
            for (int j = 7; j >= 0; --j) {
                p[j] = ti & uchar(1);
                ti >>= 1;
            }

            guo_c[i] = (!p[0] & (p[1] | p[2])) + (!p[2] & (p[3] | p[4])) +
                       (!p[4] & (p[5] | p[6])) + (!p[6] & (p[7] | p[0]));
            guo_N1[i] = (p[7] | p[0]) + (p[1] | p[2]) + (p[3] | p[4]) + (p[5] | p[6]);
            guo_N2[i] = (p[0] | p[1]) + (p[2] | p[3]) + (p[4] | p[5]) + (p[6] | p[7]);
            guo_lm[i] = (p[4] | p[5] | !p[7]) & p[6];
            guo_rm[i] = (p[0] | p[1] | !p[3]) & p[2];


            int N = guo_N1[i] < guo_N2[i] ? guo_N1[i] : guo_N2[i];
            int m_0 = guo_lm[i];
            int m_1 = guo_rm[i];

            if (guo_c[i] == 1 && (N >= 2 && N <= 3) & (m_0 == 0))
                new_value_0[i] = 1;
            else
                new_value_0[i] = 0;

            if (guo_c[i] == 1 && (N >= 2 && N <= 3) & (m_1 == 0))
                new_value_1[i] = 1;
            else
                new_value_1[i] = 0;
        }
    };

    Initializer init;
}

static void GuoHallIteration_optimized_0(cv::Mat &im) {
    cv::Mat marker = cv::Mat::zeros(im.size(), CV_8UC1);

    for (int i = 1; i < im.rows - 1; i++) {
        for (int j = 1; j < im.cols - 1; j++) {
            if (marker.at<uchar>(i, j) != 1) {
                uchar idx = (im.at<uchar>(i - 1, j) << 7) |
                            (im.at<uchar>(i - 1, j + 1) << 6) |
                            (im.at<uchar>(i, j + 1) << 5) |
                            (im.at<uchar>(i + 1, j + 1) << 4) |
                            (im.at<uchar>(i + 1, j) << 3) |
                            (im.at<uchar>(i + 1, j - 1) << 2) |
                            (im.at<uchar>(i, j - 1) << 1) |
                            im.at<uchar>(i - 1, j - 1);

                marker.at<uchar>(i, j) = new_value_0[idx];
            }
        }
    }

    im &= ~marker;
}

static void GuoHallIteration_optimized_1(cv::Mat &im) {
    cv::Mat marker = cv::Mat::zeros(im.size(), CV_8UC1);

    for (int i = 1; i < im.rows - 1; i++) {
        for (int j = 1; j < im.cols - 1; j++) {
            if (marker.at<uchar>(i, j) != 1) {
                uchar idx = (im.at<uchar>(i - 1, j) << 7) |
                            (im.at<uchar>(i - 1, j + 1) << 6) |
                            (im.at<uchar>(i, j + 1) << 5) |
                            (im.at<uchar>(i + 1, j + 1) << 4) |
                            (im.at<uchar>(i + 1, j) << 3) |
                            (im.at<uchar>(i + 1, j - 1) << 2) |
                            (im.at<uchar>(i, j - 1) << 1) |
                            im.at<uchar>(i - 1, j - 1);

                marker.at<uchar>(i, j) = new_value_1[idx];
            }
        }
    }

    im &= ~marker;
}

void GuoHallThinning_optimized(const cv::Mat &src, cv::Mat &dst) {
    CV_Assert(CV_8UC1 == src.type());
    dst = src / 255;

    cv::Mat prev = cv::Mat::zeros(src.size(), CV_8UC1);
    cv::Mat diff;

    do {
        GuoHallIteration_optimized_0(dst);
        GuoHallIteration_optimized_1(dst);
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
