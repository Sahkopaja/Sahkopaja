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

	if (hierarchy.size() > 0)
	{
		for (int i = 0; i >= 0; i = hierarchy[i][0])
		{
			cv::Moments moment = cv::moments((cv::Mat)contours[i]);
			double area = moment.m00;
			if ( area < minArea || area > maxArea )
			{
				moments.push_back(moment);
				cv::circle(tmpFrame, cv::Point(moment.m10/area, moment.m01/area), 20, cv::Scalar(255,255),2);
			}
		}
	}

	resultFrame = tmpFrame;
}

