#ifndef _MOTIONTRACK_HPP_
#define _MOTIONTRACK_HPP_

#include <opencv2/opencv.hpp>

static const int HISTORY = 50;
static const double THRESHOLD = 3.0;

static const int MIN_AREA = 400;
static const int MAX_AREA = 2000;

//MotionTrack class will wrap most motion tracking functionality, which will be called from main()
class MotionTrack
{
	private:
		cv::VideoCapture capture;

		cv::Mat background;
		cv::Mat resultFrame;

		cv::BackgroundSubtractor *bgsubtract;

		std::vector<std::vector<cv::Point> > contours;
        std::vector<cv::Vec4i> hierarchy;

        std::vector<cv::Moments> moments;

		double learningRate;
		int dilateRange, erodeRange;

        int minArea = MIN_AREA;
        int maxArea = MAX_AREA;

	public:
		MotionTrack(int _dilateRange, int _erodeRange, double _learningRate)
		{
			bgsubtract = new cv::BackgroundSubtractorMOG2( HISTORY, THRESHOLD, true );
			dilateRange = _dilateRange;
			erodeRange = _erodeRange;
			learningRate = _learningRate;
		}

		void UpdateFrame(cv::Mat newFrame);

		cv::Mat getResult() {return resultFrame.clone();}
		cv::Mat getBackground() {return background.clone();}
};

#endif
