#include <stdio.h>
#include <iostream>
#include <string>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "skeleton_filter.hpp"

using namespace std;
using namespace cv;

// Macros for time measurements
#define TS(name) int64 t_##name = getTickCount()
#define TE(name) printf("TIMER_" #name ": %.2fms\n", \
    1000.f * ((getTickCount() - t_##name) / getTickFrequency()))

const char* options =
     "{ h | help  | false | print help       }"
     "{ i | image |       | image to process }";

int main(int argc, const char** argv)
{
    // Parse command line arguments
    CommandLineParser parser(argc, argv, options);

    // If help option is given, print help message and exit
    if (parser.get<bool>("help"))
    {
        parser.printParams();
        return 1;
    }

    // Load input image
    string image_path = parser.get<string>("image");
    Mat input = imread(image_path);
    if (input.empty())
        cout << "Error: failed to open image " << image_path << endl;

    // Show input image
    imshow("Input image", input);
    waitKey(0);

    // Process image
    Mat output;
    TS(skeletonize); // start timing
    skeletonize(input, output);
    TE(skeletonize); // stop timing

    // Show output image
    imshow("Output image", output);
    waitKey(0);

    return 0;
}
