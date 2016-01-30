#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include "skeleton_filter.hpp"

using namespace std;
using namespace cv;

// Macros for time measurements
#if 1
  #define TS(name) int64 t_##name = getTickCount()
  #define TE(name) printf("TIMER_" #name ": %.2fms\n", \
    1000.f * ((getTickCount() - t_##name) / getTickFrequency()))
#else
  #define TS(name)
  #define TE(name)
#endif

const char* params =
     "{ h | help      | false | print usage           }"
     "{   | image     |       | image file to process }";

int main(int argc, const char** argv)
{
    // Parse command line arguments
    CommandLineParser parser(argc, argv, params);

    // If help flag is given, print help message and exit
    if (parser.get<bool>("help"))
    {
        parser.printParams();
        return 1;
    }

    string image_file = parser.get<string>("image");

    Mat image = imread(image_file);

    if (image.empty())
        cout << "Error: failed to open image " << image_file << endl;
    
    imshow("Input image", image);
    cout << "Show original image \nPress key on the image to start  \n";
    
    waitKey(0);
    
    cout << "Skeletonization ...\n";
    
    Mat output;
    TS(skeletonize);
    skeletonize(image, output);
    TE(skeletonize);

    imshow("Output image", output);
    cout << "Show final image \n";
    
    waitKey(0);

    return 0;
}
