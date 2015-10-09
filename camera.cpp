#include "camera.hpp"


static const int FRAME_W = 240;
static const int FRAME_H = 180;

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

void frameUpdate(bool *keepRunning, cv::Mat *_frame, std::mutex *_frameMutex, cv::VideoCapture *_cap)
{
    std::chrono::duration<double, std::milli> sleep_duration(5);
	cv::Mat tmp;
    while(*keepRunning)
    {
        _frameMutex->lock();
        _cap->read(tmp);
		*_frame = tmp;
        _frameMutex->unlock();
        std::this_thread::sleep_for(sleep_duration);
    }
    return;
}

