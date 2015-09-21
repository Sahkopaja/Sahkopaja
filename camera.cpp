#include "camera.hpp"
#include <exception>

#include <opencv2/opencv.hpp>

static const int FRAME_W = 640;
static const int FRAME_H = 480;

cv::VideoCapture initCamera()
{
	cv::VideoCapture cap(0);

	if (!cap.isOpened())
	{
		throw new std::runtime_error("Camera couldn't be opened.");
	}

    cap.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_W);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_H);
	
	return cap;
}
