#include "motiontrack.hpp"
#include <stdio.h>
void MotionTrack::UpdateFrame(cv::Mat newFrame)
{
	cv::Mat tmpFrame = newFrame.clone();
	
    /*
	//Simplify the frame
	cv::Mat dilateElement = getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(dilateRange, dilateRange));
	cv::Mat erodeElement = getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(erodeRange, erodeRange));
	cv::dilate(tmpFrame, tmpFrame, cv::Mat());
	cv::erode(tmpFrame, tmpFrame, cv::Mat());
    */

    cv::GaussianBlur(tmpFrame, tmpFrame, cv::Size(blurStrength, blurStrength), 0, 0, cv::BORDER_DEFAULT);

	//Subtract the static background from the frame
	bgsubtract->operator()(tmpFrame, tmpFrame, learningRate);
	bgsubtract->getBackgroundImage(background);
    
	//Simplify the frame
	cv::Mat dilateElement = getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(blurStrength,blurStrength));
	cv::Mat erodeElement = getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(blurStrength,blurStrength));
	cv::dilate(tmpFrame, tmpFrame, cv::Mat());
	cv::erode(tmpFrame, tmpFrame, cv::Mat());
 
    cv::GaussianBlur(tmpFrame, tmpFrame, cv::Size(blurStrength, blurStrength), 0, 0, cv::BORDER_DEFAULT);
	cv::threshold(tmpFrame, tmpFrame, 128.0, 255.0, cv::THRESH_BINARY);

	cv::findContours( tmpFrame.clone(), contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

	for (unsigned i = 0; i < contours.size(); i++)
	{
		double area = cv::contourArea(contours[i], false);
		if ( area < minArea || area > maxArea)
		{
			contours.erase(contours.begin() + i);
		}
	}

	resultFrame = tmpFrame;
}

