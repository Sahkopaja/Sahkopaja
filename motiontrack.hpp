#ifndef _MOTIONTRACK_HPP_
#define _MOTIONTRACK_HPP_

#include <opencv2/opencv.hpp>

static const int HISTORY = 50;
static const double THRESHOLD = 0.75;

//MotionTrack class will wrap most motion tracking functionality, which will be called from main()
class MotionTrack
{
	private:
		cv::VideoCapture capture;

		cv::Mat background;
		cv::Mat resultFrame;

		cv::BackgroundSubtractor *bgsubtract;

		std::vector<std::vector<cv::Point> > contours;

		double learningRate;
		int dilateRange, erodeRange;

	public:
		MotionTrack(int _dilateRange, int _erodeRange, double _learningRate)
		{
			bgsubtract = new cv::BackgroundSubtractorMOG2( HISTORY, THRESHOLD, false );
			dilateRange = _dilateRange;
			erodeRange = _erodeRange;
			learningRate = _learningRate;
		}

		void UpdateFrame(cv::Mat newFrame);

		cv::Mat getResult() {return resultFrame.clone();}
		cv::Mat getBackground() {return background.clone();}
};

#endif
