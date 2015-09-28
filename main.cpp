#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <thread>
#include <mutex>
#include <chrono>

#include "camera.hpp"
#include "motiontrack.hpp"

int main(int argc, char** argv)
{
    cv::VideoCapture cap = initCamera();

    cv::namedWindow("Alkuperainen", cv::WINDOW_NORMAL);
    cv::namedWindow("Suodatettu", cv::WINDOW_NORMAL);

    bool keepRunning = true;
    
    std::mutex frameMutex;
    cv::Mat _frame;
   
    cap.read(_frame);
    std::thread frameThread(frameUpdate, &keepRunning, &_frame, &frameMutex, &cap);

	MotionTrack motion;

    cv::Mat frame;
    while(keepRunning)
    {
        frameMutex.lock();
        frame = _frame;
        frameMutex.unlock();
	
		motion.UpdateFrame(frame);
        
        cv::imshow("Alkuperainen", frame);
        cv::imshow("Suodatettu", motion.getResult());

        if (cv::waitKey(1) == 113) keepRunning = false;
    }
	
    frameThread.join();
    
	return 0;
}

