#include "skeleton_filter.hpp"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

void skeletonize(const cv::Mat &input, cv::Mat &output, bool save_images)
{
    TS(total);

    TS(imwrite_0);
        if (save_images) cv::imwrite("0-input.png", input);
    TE(imwrite_0);

    // Convert to grayscale
    TS(ConvertColor_BGR2GRAY_BT709_all);
        cv::Mat gray_image;

        TS(ConvertColor_BGR2GRAY_BT709_only);
        ConvertColor_BGR2GRAY_BT709(input, gray_image);
        TE(ConvertColor_BGR2GRAY_BT709_only);

        if (save_images) cv::imwrite("1-convertcolor.png", gray_image);
    TE(ConvertColor_BGR2GRAY_BT709_all);

    // Downscale input image
    TS(ImageResize_all);
        cv::Mat small_image;
        cv::Size small_size(input.cols / 1.5, input.rows / 1.5);

        TS(ImageResize_only);
        ImageResize(gray_image, small_image, small_size);
        TE(ImageResize_only);

        if (save_images) cv::imwrite("2-resize.png", small_image);
    TE(ImageResize_all);

    // Binarization and inversion
    TS(threshold_all);
        TS(threshold_only);
        cv::threshold(small_image, small_image, 128, 255, cv::THRESH_BINARY_INV);
        TE(threshold_only);
        if (save_images) cv::imwrite("3-threshold.png", small_image);
    TE(threshold_all);

    // Thinning
    TS(GuoHallThinning_all);
        cv::Mat thinned_image;
        TS(GuoHallThinning_only);
        GuoHallThinning(small_image, thinned_image);
        TE(GuoHallThinning_only);
        if (save_images) cv::imwrite("4-thinning.png", thinned_image);
    TE(GuoHallThinning_all);

    // Back inversion
    TS(Back_inversion_all);
        TS(Back_inversion_only);
        output = 255 - thinned_image;
        TE(Back_inversion_only);
        if (save_images) cv::imwrite("5-output.png", output);
    TE(Back_inversion_all);

    TE(total);
}
