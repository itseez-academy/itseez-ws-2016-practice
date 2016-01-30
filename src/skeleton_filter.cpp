#include "skeleton_filter.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <time.h>

using namespace std;
using namespace cv;

void skeletonize(const Mat &input, Mat &output)
{
    output = input.clone();
}
