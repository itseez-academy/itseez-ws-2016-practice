#include <iostream>
#include <string>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "skeleton_filter.hpp"

using namespace std;
using namespace cv;

const char* options =
	"{ i | image |       | image to process         }"
	"{ s | save  | false | save intermediate images }"
	"{ h | help  | false | print help               }";

int main(int argc, const char** argv)
{
	// Parse command line arguments
	CommandLineParser parser(argc, argv, options);

	// If help option is given, print help message and exit
	if (parser.get<bool>("help"))
	{
		parser.printParams();
		return 0;
	}

	// Load input image
	string image_path = parser.get<string>("image");
	Mat input = imread(image_path);
	if (input.empty())
		cout << "Error: failed to open image " << image_path << endl;
	else
		cout << "Successfully opened image " << image_path << endl;

	// Show input image
	imshow("Input image", input);
	waitKey(1000);

	// Check if we need to save intermediate images
	bool save_images = parser.get<bool>("save");
	if (save_images)
		cout << "Image saving is ENABLED" << endl;
	else
		cout << "Image saving is DISABLED" << endl;

	// Process image
	Mat output;
	skeletonize(input, output, save_images);

	// Show output image
	//imshow("Output image", output);
	//waitKey(1000);

	namedWindow("Input image", WINDOW_NORMAL);
	imshow("Input image", input);
	waitKey(1);
	/*...*/
	waitKey();  // Wait key down.
	
	return 0;
}
