#include "skeleton_filter.hpp"
#include "internal.hpp"

#include "opencv2/imgproc/imgproc.hpp"
#include <time.h>

using namespace std;
using namespace cv;

void skeletonize(const Mat &input, Mat &output)
{
    // Convert to grayscale
    Mat gray_image;
    // cvtColor(input, gray_image, CV_BGR2GRAY);
    ConvertColor_BGR2GRAY_BT709(input, gray_image);

    // Downscale input image
    Mat small_image;
    Size small_size(input.cols / 2, input.rows / 2);
    // resize(gray_image, small_image, small_size);
    ImageResize(gray_image, small_image, small_size);

    output = small_image;

    // Thinning
}
