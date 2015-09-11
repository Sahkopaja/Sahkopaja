#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <thread>
#include <mutex>
#include <chrono>
#include <string>

#include "camera.hpp"
#include "motiontrack.hpp"

int main(int argc, char** argv)
{
	bool debugMode = false;

	//Not a good way to handle this, but will do for now since we probably won't have any more commmand line arguments
	if (argc > 1 && std::string(argv[1]) == "--DEBUG") debugMode = true;
	
    cv::VideoCapture cap = initCamera();
	
	if (debugMode)
	{
		cv::namedWindow("Alkuperainen", cv::WINDOW_NORMAL);
		cv::namedWindow("Suodatettu", cv::WINDOW_NORMAL);
		cv::namedWindow("Tausta", cv::WINDOW_NORMAL);
	}

    bool keepRunning = true;
    
    std::mutex frameMutex;
    cv::Mat _frame;

	cap.read(_frame);

    std::thread frameThread(frameUpdate, &keepRunning, &_frame, &frameMutex, &cap);

	MotionTrack motion(8, 32, 0.05);

    cv::Mat frame;
	int pressed;
    while(keepRunning)
    {
        frameMutex.lock();
        frame = _frame;
        frameMutex.unlock();
	
		motion.UpdateFrame(frame);
		
		if (debugMode)
		{	
			cv::imshow("Alkuperainen", frame);
			cv::imshow("Suodatettu", motion.getResult());
			cv::imshow("Tausta", motion.getBackground());
		}

		pressed = cv::waitKey(1);
        if ((char)pressed == 'q') keepRunning = false;
    }
	
    frameThread.join();
    
	return 0;
}

