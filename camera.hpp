#ifndef _CAMERA_HPP_
#define _CAMERA_HPP_

#include <opencv2/opencv.hpp>

#include <thread>
#include <mutex>
#include <chrono>

cv::VideoCapture initCamera();
void frameUpdate(bool *keepRunning, cv::Mat *_frame, std::mutex *_frameMutex, cv::VideoCapture *_cap);

#endif
