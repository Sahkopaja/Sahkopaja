#ifndef _MOTIONTRACK_HPP_
#define _MOTIONTRACK_HPP_

#include <opencv2/opencv.hpp>
#include <map>
#include <utility>

#include "camera.hpp"

static const int HISTORY = 50;
static const double THRESHOLD = 3.0;

static const int MIN_AREA = 1000;
static const int MAX_AREA = 40000;

static const int HISTORY_LENGTH = 10;

static const int MOMENT_MERGE_THRESH = 50;

struct Target;

//MotionTrack class will wrap most motion tracking functionality, which will be called from main()
class MotionTrack
{
	private:
		cv::VideoCapture capture;

		cv::Mat background;
		cv::Mat resultFrame;
		cv::Mat goalFrame;

		cv::BackgroundSubtractor *bgsubtract;

		std::vector<std::vector<cv::Point> > contours;
        std::vector<cv::Vec4i> hierarchy;

        std::vector<cv::Moments> moments;
		std::vector<Target> targets;

		std::pair<double, double> targetLocation, targetVelocity;

		std::vector<std::pair<double, double> > targetHistory;

		double learningRate;
		int blurStrength;

        int minArea;
        int maxArea;

		int history_length;

	public:
		MotionTrack(Preferences *pref)
		{
			int history = pref->readInt("background_history", HISTORY);
			double threshold = pref->readDouble("background_threshold", THRESHOLD);
			bgsubtract = new cv::BackgroundSubtractorMOG2( history, threshold, true );
			
			int _blurStrength = pref->readInt("background_blur", 16);
			double _learningRate = pref->readDouble("background_learn", 0.05);
			blurStrength = _blurStrength * 2 + 1;
			learningRate = _learningRate;

			minArea = pref->readInt("motion_detection_min_area", MIN_AREA);
			maxArea = pref->readInt("motion_detection_max_area", MAX_AREA);

			history_length = pref->readInt("motion_detection_target_history", HISTORY_LENGTH);
		}

		std::vector<Target> MergeMoments(std::vector<cv::Moments> moments);

		void UpdateFrame(cv::Mat newFrame);

		cv::Mat getResult() {return resultFrame.clone();}
		cv::Mat getBackground() {return background.clone();}
		cv::Mat getGoalFrame() {return goalFrame.clone();}

		std::pair<double, double> getTargetLocation() {return targetLocation;}
		std::pair<double, double> getTargetVelocity() {return targetVelocity;}
		
		bool targetFound;
};

//Simple struct to contain targets to shoot at
struct Target
{
	private:
		double x, y, w, h;
	
	public:
		Target(double _x, double _y, double _w, double _h) : x(_x), y(_y), w(_w), h(_h) {}

		double getX() {return x;}
		double getY() {return y;}
		double getW() {return w;}
		double getH() {return h;}
		double getArea() {return w*h;}
};

#endif
