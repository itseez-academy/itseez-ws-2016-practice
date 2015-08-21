#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include "retro_filter.hpp"

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
     "{ h | help      | false | print usage                                   }"
     "{   | video     |       | video file to detect on                       }"
     "{   | camera    | false | whether to detect on video stream from camera }"
     "{   | border    |       | image to use as a border                      }"
     "{   | scratches |       | image to use for scratches                    }";

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
    string video_file = parser.get<string>("video");
    bool use_camera = parser.get<bool>("camera");
    string border = parser.get<string>("border");
    string scratches = parser.get<string>("scratches");

    RetroFilter::Parameters params;
    params.fuzzyBorder = imread(border, 0);
    params.scratches   = imread(scratches, 0);

    if (params.fuzzyBorder.empty())
        cout << "Error: failed to open image to use as a border: " << border << endl;
    if (params.scratches.empty())
        cout << "Error: failed to open image to use for scratches: " << scratches << endl;
    if (params.fuzzyBorder.empty() || params.scratches.empty())
        return 1;

    VideoCapture capture;
    if (!video_file.empty())
    {
        capture.open(video_file);
        if (!capture.isOpened())
        {
            cout << "Error: failed to open video stream for: " << video_file << endl;
            return 1;
        }
    }
    else if (use_camera)
    {
        capture.open(0);
        if (!capture.isOpened())
        {
            cout << "Error: failed to open video stream for camera #0" << endl;
            return 1;
        }
    }
    else
    {
        cout << "Error: declare a source of images" << endl;
        parser.printParams();
        return 1;
    }

    Mat frame;
    capture >> frame;
    capture >> frame;

    if (frame.empty())
    {
        // empty video; lets consider this to be OK
        return 0;
    }

    params.frameSize   = frame.size();
    RetroFilter filter(params);

    for(;;)
    {
        Mat retroFrame;
        TS(filter);
        filter.applyToVideo(frame, retroFrame);
        TE(filter);

        imshow("Original Movie", frame);
        imshow("Retro Movie", retroFrame);
        char c = (char) waitKey(1);
        if( c == 27 ) // Esc
            break;

        capture >> frame;
        if(frame.empty()) break;
    }

    return 0;
}
