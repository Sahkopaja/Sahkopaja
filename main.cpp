#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <thread>
#include <mutex>
#include <chrono>

#include "camera.hpp"

void frameUpdate(bool *keepRunning, cv::Mat *_frame, std::mutex *_frameMutex, cv::VideoCapture *_cap)
{
    std::chrono::duration<double, std::milli> sleep_duration(5);
    while(keepRunning)
    {
        _frameMutex->lock();
        _cap->read(*_frame);
        _frameMutex->unlock();
        std::this_thread::sleep_for(sleep_duration);
    }
    printf("frameUpdate exited\n");
    return;
}

int main(int argc, char** argv)
{
    cv::VideoCapture cap = initCamera();    
    cv::Mat back;
    cv::Mat front;
    
    cv::BackgroundSubtractorMOG bgsub;

    std::vector<std::vector<cv::Point> > contours;

    cv::namedWindow("Alkuperainen", cv::WINDOW_NORMAL);
    cv::namedWindow("Suodatettu", cv::WINDOW_NORMAL);

    bool keepRunning = true;
    
    std::mutex frameMutex;
    cv::Mat _frame;
   
    cap.read(_frame);
    std::thread frameThread(frameUpdate, &keepRunning, &_frame, &frameMutex, &cap);

    cv::Mat frame;
    cv::Mat resultFrame;
    while(keepRunning)
    {
        frameMutex.lock();
        frame = _frame;
        frameMutex.unlock();
        
        cvtColor(frame, frame, CV_BGR2GRAY);

        cv::erode(frame,frame,cv::Mat());
        cv::dilate(frame,frame,cv::Mat());
        
        /* --MOG-- */
        bgsub.operator()(frame, resultFrame);
        bgsub.getBackgroundImage(back);

        //cv::findContours(front, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
        //cv::drawContours(cpFrame, contours, -1, cv::Scalar(0, 0, 255), 2);

        cv::imshow("Alkuperainen", frame);
        cv::imshow("Suodatettu", resultFrame);

        if (cv::waitKey(1) >= 0) keepRunning = false;
    }

    frameThread.join();

    return 0;
}

