#include <iostream> // for standard I/O
#include <string>   // for strings
#include <iomanip>  // for controlling float print precision
#include <sstream>  // string to number conversion

#include <opencv2/core/core.hpp>        // basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc/imgproc.hpp>  // OpenCV image processing
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O

using namespace std;
using namespace cv;

/*---------------------------------------------------------------------------
TASK 1 
	!! Only add code to the reserved places.
	!! The resulting program must not print anything extra on any output (nothing more than the prepared program framework).
*/


/*	TASK 1.1 - Convert image from RGB to GRAYSCALE
	Iterate over pixels and convert RGB values to GRASCALE value.
	Convert final <float> value to <unsigned char> value.
	Note that the default color format in OpenCV is often referred to as RGB but it is actually BGR (the bytes are reversed).

	Allowed Mat attributes, methods and OpenCV functions for task solution are:
		Mat:: ... rows, cols, step(), size(), at<>(), zeros(), ones(), eye()

*/
int rgb2gray( Mat& bgr, Mat& gray )
{
	gray = cv::Mat::zeros( bgr.size(), CV_8UC1 );

	/* ***** Working area - begin ***** */

	for(int i = 0; i < bgr.rows; i++){
		for(int j = 0; j < bgr.cols; j++){
			// Load 3 bytes into a vector
			cv::Vec3b pixel = bgr.at<cv::Vec3b>(i,j);
			// BGR
			unsigned char blue = pixel[0];
			unsigned char green = pixel[1];
			unsigned char red = pixel[2];
			
			// Round the floating point value, and cast it to unsigned char
			unsigned char gray_scale = static_cast<unsigned char>(std::round( 0.299*red+0.587*green+0.114*blue));
			// Save it to grayscale matrix
			gray.at<unsigned char>(i,j) = gray_scale;
		}
	}
	/* ***** Working area - end ***** */

	return 0;
}


/* 	TASK 1.2 - Convolution 
	The input is a grayscale image (0-255) and a 3x3 float kernel (CV_32FC1).
    Leave the border values of the resulting image at 0.
	Implement manually by passing the image, for each pixel go through its surroundings and perform convolution with a 3x3 core.
    The resulting value must be normalized.

	Allowed Mat attributes, methods and OpenCV functions for task solution are:
		Mat:: ... rows, cols, step(), size(), at<>(), zeros(), ones(), eye()
    
*/
int convolution( cv::Mat& gray, const cv::Mat& kernel, cv::Mat& dst )
{
	dst = cv::Mat::zeros( gray.size(), CV_32FC1 );

	if( kernel.rows != 3 || kernel.cols != 3 )
		return 1;

	/* *****  Working area - begin ***** */

		/*
		iterate over 3x3 neighbourhood of the current point and:
			1. calculate the convolution of a local area with a convolution kernel
			2. normalize the result value
			3. save to the output image
		*/


	// clone kernel and flip him vertically and horizontally
	cv::Mat kernel_copy = kernel.clone();

	std::swap(kernel_copy.at<float>(0, 0), kernel_copy.at<float>(2, 2));
    std::swap(kernel_copy.at<float>(0, 2), kernel_copy.at<float>(2, 0));
    std::swap(kernel_copy.at<float>(0, 1), kernel_copy.at<float>(2, 1));
    std::swap(kernel_copy.at<float>(1, 0), kernel_copy.at<float>(1, 2));

	// sum the absolute kernel values for later nomralization
	float kernel_sum = 0; 
	for (int i = 0; i < kernel_copy.rows; i++){
		for(int j = 0; j < kernel_copy.cols; j++){
			kernel_sum += abs( kernel_copy.at<float>(i,j));
		}
	}

	// iterate over image, but not the edges of the image
	for(int i = 1; i < gray.rows - 1; i++){
		for(int j = 1; j < gray.cols - 1; j++){
			float new_value = 0;
			//iterate over kernel
			for(int k = -1; k <= 1; k++){
				for(int l = -1; l <= 1; l++){
					new_value += gray.at<unsigned char>(i+k,j+l) * kernel_copy.at<float>(1+k,l+1);
				}
			}

			// normalize and store the result in the destination matrix
			if (kernel_sum != 0) { // avoid division by zero (if the kernel is empty)
				new_value /= kernel_sum;
			}
			dst.at<float>(i,j) = new_value;
		}
	}

	/* ***** Working area - end ***** */

	return 0;
}

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// support functions
void checkDifferences( const cv::Mat test, const cv::Mat ref, std::string tag, bool save );
//---------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		cout << "Not enough parameters" << endl;
		return -1;
	}

	// load image, use first argument, load as RGB image
	Mat img_rgb = imread(argv[1], IMREAD_COLOR);
	if(img_rgb.empty() )                              
	{
		cout <<  "Could not open or find the original image" << endl ;
		return 1;
	}

	//---------------------------------------------------------------------------
	// TASK: RGB convert to GRAYSCALE image
	
	Mat img_gray, img_ref;
	
	// compute solution using your own function
	rgb2gray(img_rgb, img_gray);
	
	// compute reference solution
	cvtColor(img_rgb, img_ref, cv::COLOR_BGR2GRAY);

	// compute and report differences
	checkDifferences( img_gray, img_ref, "rgb2gray", true );


	//---------------------------------------------------------------------------
	// TASK: convolution

	float ker[9] = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };
	cv::Mat kernel( 3, 3, CV_32FC1, ker );
	cv::Mat conv_res, conv_ref;

	img_gray = img_ref;

	// compute solution using your own function
	convolution( img_gray, kernel, conv_res );

	// compute reference solution
	cv::flip( kernel, kernel, -1 );
	cv::filter2D( img_gray, conv_ref, CV_32F, kernel );
	// normalize convolution output
	conv_ref *= 1.f/(cv::sum(abs(kernel)).val[0] + 0.000000001);

	// since the filter2D function also calculates the values at the edges of the output image (and we don't for simplicity)
	// erase the edge values of the image before comparison
	cv::rectangle( conv_ref, cv::Point(0,0), cv::Point(conv_ref.cols-1,conv_ref.rows-1), cv::Scalar::all(0), 1 );

	// compute and report differences
	checkDifferences( conv_res, conv_ref, "convolution", true );

	//---------------------------------------------------------------------------

  
	return 0;
}
    



//---------------------------------------------------------------------------
void checkDifferences( const cv::Mat test, const cv::Mat ref, std::string tag, bool save )
{
	double mav = 255., err = 255., nonzeros = 1000.;

	if( !test.empty() && !ref.empty() ) {
		cv::Mat diff;
		cv::absdiff( test, ref, diff );
		cv::minMaxLoc( diff, NULL, &mav );
		nonzeros = 1. * cv::countNonZero( diff ); // / (diff.rows*diff.cols);
		err = (nonzeros > 0 ? ( cv::sum(diff).val[0] / nonzeros ) : 0);

		if( save ) {
			diff *= 255;
			cv::imwrite( (tag+".0.ref.png").c_str(), ref );
			cv::imwrite( (tag+".1.test.png").c_str(), test );
			cv::imwrite( (tag+".2.diff.png").c_str(), diff );
		}
	}

	printf( "%s_avg_cnt_max, %.1f, %.0f, %.0f, ", tag.c_str(), err, nonzeros, mav );
	
	return;
}
//---------------------------------------------------------------------------



