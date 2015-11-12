#include <stdio.h>
#include <signal.h>

#include <opencv2/opencv.hpp>
#include <thread>
#include <mutex>
#include <chrono>
#include <string>

#include "camera.hpp"
#include "motiontrack.hpp"
#include "preferences.hpp"
#include "hardware.hpp"

bool keepRunning = true;

void interruptHandler(int)
{
	keepRunning = false;
}

int main(int argc, char** argv)
{
	struct sigaction act;
	act.sa_handler = interruptHandler;
	sigaction(SIGINT, &act, NULL);

	bool debugMode = false;
	std::string configFile = "preferences.json";

	if (argc > 1)
	{
		configFile = std::string(argv[1]);
	}
	
	Preferences preferences(configFile);
	debugMode = (preferences.readInt("debug_mode", 1) != 0);

    cv::VideoCapture cap = initCamera(&preferences);
	
	if (debugMode)
	{
		cv::namedWindow("Video", cv::WINDOW_NORMAL);
		cv::namedWindow("Suodatettu", cv::WINDOW_NORMAL);
		cv::namedWindow("Tausta", cv::WINDOW_NORMAL);
	}

	cv::Mat _frame;

	cap.read(_frame);

    std::mutex frameMutex;
    std::thread frameThread(frameUpdate, &keepRunning, &_frame, &frameMutex, &cap);
	
	double targetX, targetY;
	GPIOState gpio(&preferences);
	std::mutex hwMutex;
	std::thread hwThread = gpio.runThread(&keepRunning, &hwMutex, &targetX, &targetY);

	MotionTrack motion(&preferences);

    cv::Mat frame;
	int pressed;

	std::pair<double, double> targetLocation;

    while(keepRunning)
    {
        frameMutex.lock();
        frame = _frame;
        frameMutex.unlock();
	
		motion.UpdateFrame(frame);
		if (motion.targetFound)
		{
			targetLocation = motion.getTargetLocation();
			targetX = targetLocation.first;
			targetY = targetLocation.second;
		}
		
		if (debugMode)
		{	
			cv::imshow("Video", motion.getGoalFrame());
			cv::imshow("Suodatettu", motion.getResult());
			cv::imshow("Tausta", motion.getBackground());
		}

		pressed = cv::waitKey(1);
        if ((char)pressed == 'q') keepRunning = false;
    }
	
	printf("\nExiting...\n");

    frameThread.join();
	hwThread.join();
    
	return 0;
}

