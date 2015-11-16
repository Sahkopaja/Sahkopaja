#include <stdio.h>
#include <signal.h>

#include <opencv2/opencv.hpp>
#include <thread>
#include <mutex>
#include <chrono>
#include <string>
#include <vector>

#include "camera.hpp"
#include "motiontrack.hpp"
#include "preferences.hpp"
#include "hardware.hpp"

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
			return getTargetValidated();
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

		//if the sample ratio exceeds given minimum sample ratio, return true. Else return false.
		bool getTargetValidated()
		{
			if (getSampleRatio() >= minimumSampleRatio)
			{
				return true;
			}
			return false;
		}
};

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

	unsigned sampleSize = preferences.readInt("target_sample_size", 0);
	double minimumSampleRatio = preferences.readDouble("target_sample_ratio", 0.9);
	bool targetFound, targetConfirmed;

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

	double targetX = -1, targetY = -1;
	GPIOState gpio(&preferences);
	std::mutex hwMutex;
	std::thread hwThread = gpio.runThread(&keepRunning, &hwMutex, &targetX, &targetY, &targetConfirmed);

	MotionTrack motion(&preferences);

	cv::Mat frame;
	int pressed;

	TargetValidator validator = TargetValidator(sampleSize, minimumSampleRatio);
	std::pair<double, double> targetLocation;

    while(keepRunning)
    {
        frameMutex.lock();
        frame = _frame;
        frameMutex.unlock();

		motion.UpdateFrame(frame);
		targetFound = motion.targetFound;
		targetConfirmed = validator.updateSamples(targetFound);

		if (targetConfirmed && debugMode)
		{
			printf("Target confirmed\n");
		}
		else if (debugMode)
		{
			printf("Target not confirmed\n");
		}

		if (targetFound)
		{
			hwMutex.lock();
			targetLocation = motion.getTargetLocation();
			targetX = targetLocation.first;
			targetY = targetLocation.second;
			hwMutex.unlock();
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

