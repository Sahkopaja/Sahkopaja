#ifndef _MOTIONTRACK_HPP_
#define _MOTIONTRACK_HPP_

#include <opencv2/opencv.hpp>

//Class will wrap most motion tracking functionality, which will be called from main()
class MotionTrack
{
	private:
		cv::VideoCapture capture;

		cv::Mat backGround;
		cv::Mat resultFrame;

		cv::BackgroundSubtractor bgsubtract;

		std::vector<std::vector<cv::Point> > contours;

	public:
		MotionTrack()
		{
			bgsubtract = cv::BackgroundSubtractorMOG();
		}

		void UpdateFrame(cv::Mat newFrame);

		cv::Mat getResult() {return resultFrame.clone();}
		cv::Mat getBackground() {return backGround.clone();}
};

#endif
