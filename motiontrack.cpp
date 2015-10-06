#include "motiontrack.hpp"

void MotionTrack::UpdateFrame(cv::Mat newFrame)
{
	cv::Mat tmpFrame = newFrame.clone();
	
	//Simplify the frame
	cv::Mat dilateElement = getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(dilateRange, dilateRange));
	cv::Mat erodeElement = getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(erodeRange, erodeRange));
	
	cv::dilate(tmpFrame, tmpFrame, dilateElement);
	cv::erode(tmpFrame, tmpFrame, erodeElement);

	//Subtract the static background from the frame
	bgsubtract->operator()(tmpFrame, resultFrame, learningRate);
	bgsubtract->getBackgroundImage(background);
}

