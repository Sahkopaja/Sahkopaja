#include "motiontrack.hpp"

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

    cv::GaussianBlur(tmpFrame, tmpFrame, cv::Size(3, 3), 3, 0, cv::BORDER_DEFAULT);

	//Subtract the static background from the frame
	bgsubtract->operator()(tmpFrame, resultFrame, learningRate);
	bgsubtract->getBackgroundImage(background);
    
    cv::findContours( resultFrame, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
    
    for (unsigned i = 0; i >= 0; i = hierarchy[i][0])
    {
        cv::Moments moment = cv::moments((cv::Mat)contours[i][0]);
        moments.push_back(moment);
    }


}

