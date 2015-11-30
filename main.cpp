#include <stdio.h>
#include <signal.h>

#include <opencv2/opencv.hpp>
#include <thread>
#include <mutex>
#include <chrono>
#include <string>
#include <vector>
#include <iostream>

#include "camera.hpp"
#include "motiontrack.hpp"
#include "preferences.hpp"
#include "hardware.hpp"

//Main loop works as a FSM, this defines possible states
typedef enum
{
	RESET,
	ACQUIRE,
	ELIMINATE
} ProgramState;

//This class essentially decides whether to shoot a potential found target
class TargetValidator
{
	private:
		double minimumSampleRatio;
		std::vector<bool> samples;
		unsigned frameIteration;

	public:
		//minimum of 1 frames in minimumSampleRatio mandatory
		TargetValidator(unsigned _sampleSize, double _minimumSampleRatio)
		{
			minimumSampleRatio = _minimumSampleRatio;
			frameIteration = 0u;
			samples.reserve(_sampleSize);

			if(_sampleSize == 0u)
			{
				_sampleSize = 1u;
			}

			for (unsigned i = 0; i < _sampleSize; i++)
			{
				samples.push_back(false);
			}
		}

		//updates the samples and returns true or false whether target was validated
		bool updateSamples(bool lastSample)
		{
			if ( !(frameIteration < samples.size()) )
			{
				frameIteration = 0u;
			}

			samples[frameIteration] = lastSample;
			frameIteration++;
			return getSampleRatio() >= minimumSampleRatio;
		}

		//how many of the samples are true
		double getSampleRatio()
		{
			unsigned sampleAmount = 0u;
			for (unsigned i=0; i < samples.size(); i++)
			{
				if (samples[i])
				{
					sampleAmount++;
				}
			}
			return (double) sampleAmount/samples.size();
		}
};

bool keepRunning = true;

//Handles Ctrl-C etc. signals
void interruptHandler(int)
{
	keepRunning = false;
}

int main(int argc, char** argv)
{
	struct sigaction act;
	act.sa_handler = interruptHandler;
	sigaction(SIGINT, &act, NULL);

	std::string configFile = "preferences.json";

	if (argc > 1)
	{
		configFile = std::string(argv[1]);
	}

	Preferences preferences(configFile);
	bool debugMode = (preferences.readInt("debug_mode", 1) != 0);
	bool disableShooting = (preferences.readInt("disable_shooting", 0) != 0);

	unsigned sampleSize = preferences.readInt("target_sample_size", 0);
	double minimumSampleRatio = preferences.readDouble("target_sample_ratio", 0.9);
	bool targetFound, targetConfirmed;

	cv::VideoCapture cap = initCamera(&preferences);

	if (debugMode)
	{
		//Setup video windows
		cv::namedWindow("Video", cv::WINDOW_NORMAL);
		cv::namedWindow("Suodatettu", cv::WINDOW_NORMAL);
		cv::namedWindow("Tausta", cv::WINDOW_NORMAL);
	}

	//Set up the thread to read frames from camera feed
	cv::Mat _frame;
	cap.read(_frame);
    std::mutex frameMutex;
    std::thread frameThread(frameUpdate, &keepRunning, &_frame, &frameMutex, &cap);

	//Set up the thread handling hardware interaction
	double targetX = -1, targetY = -1;
	GPIOState gpio(&preferences);
	std::mutex hwMutex;
	bool startShooting = false;
	bool shootingDone = false;
	std::thread hwThread = gpio.runThread(&keepRunning, &hwMutex, &targetX, &targetY, &startShooting, &shootingDone, &disableShooting);

	MotionTrack motion(&preferences);

	cv::Mat frame;
	int pressed;

	TargetValidator validator = TargetValidator(sampleSize, minimumSampleRatio);
	std::pair<double, double> targetLocation;

	ProgramState state = RESET;

	//The main loop of the program
    while(keepRunning)
    {
		//Read a frame from camera thread
        frameMutex.lock();
        frame = _frame;
        frameMutex.unlock();

		//Do motion detection calculations
		motion.UpdateFrame(frame);
		targetFound = motion.targetFound;
		targetConfirmed = validator.updateSamples(targetFound);

		//Reset gun position
		if (state == ProgramState::RESET)
		{
			targetX = -1.0;
			targetY = -1.0;

			if (!targetConfirmed)
			{
				state = ProgramState::ACQUIRE;
			}
		}
		//Move to elimination phase if target is found
		else if (state == ProgramState::ACQUIRE)
		{
			if (targetConfirmed)
			{
				state = ProgramState::ELIMINATE;
				startShooting = true;
			}
		}
		//Aim at the target if found
		else if (state == ProgramState::ELIMINATE)
		{
			hwMutex.lock();
			targetLocation = motion.getTargetLocation();
			targetX = targetLocation.first;
			targetY = targetLocation.second;

			if (shootingDone)
			{
				startShooting = false;
				shootingDone = false;
				state = ProgramState::RESET;
			}

			hwMutex.unlock();
		}

		//Update the windows
		if (debugMode)
		{
			cv::imshow("Video", motion.getGoalFrame());
			cv::imshow("Suodatettu", motion.getResult());
			cv::imshow("Tausta", motion.getBackground());
		}

		//For quitting the program press q (only works in debug mode, use ctrl-c in other cases)
		pressed = cv::waitKey(1);
        if ((char)pressed == 'q') keepRunning = false;
    }

	std::cout << "\nExiting...\n";

	//Wait for other threads to close
    frameThread.join();
	hwThread.join();

	return 0;
}

