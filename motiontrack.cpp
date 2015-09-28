#include "motiontrack.hpp"

void MotionTrack::UpdateFrame(cv::Mat newFrame)
{
	cv::Mat tmpFrame = newFrame.clone();

	cvtColor(tmpFrame, tmpFrame, CV_BGR2GRAY);

	cv::erode(tmpFrame, tmpFrame, cv::Mat());
	cv::dilate(tmpFrame, tmpFrame, cv::Mat());
	
	/* --MOG-- */
	bgsubtract.operator()(tmpFrame, tmpFrame);
	bgsubtract.getBackgroundImage(backGround);
	
	resultFrame = tmpFrame;

	//cv::findContours(front, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	//cv::drawContours(cpFrame, contours, -1, cv::Scalar(0, 0, 255), 2);
}

