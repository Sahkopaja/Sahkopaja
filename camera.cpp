#include "camera.hpp"
#include "preferences.hpp"

static const int FRAME_W = 240;
static const int FRAME_H = 180;

cv::VideoCapture initCamera(Preferences *pref)
{
	cv::VideoCapture cap(0);

	if (!cap.isOpened())
	{
		throw new std::runtime_error("Camera couldn't be opened.");
	}

	int frame_width = pref->readInt("camera_frameW", FRAME_W);
	int frame_height = pref->readInt("camera_frameH", FRAME_H);

	cap.set(CV_CAP_PROP_FRAME_WIDTH, frame_width);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, frame_height);
	
	return cap;
}

void frameUpdate(bool *keepRunning, cv::Mat *_frame, std::mutex *_frameMutex, cv::VideoCapture *_cap)
{
	//Keep reading frames from camera feed
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

    _cap->release();
    return;
}

