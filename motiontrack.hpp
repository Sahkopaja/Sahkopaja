#ifndef _MOTIONTRACK_HPP_
#define _MOTIONTRACK_HPP_

#include <opencv2/opencv.hpp>

static const int HISTORY = 50;
static const double THRESHOLD = 3.0;

static const int MIN_AREA = 1000;
static const int MAX_AREA = 40000;

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

		unsigned targetX, targetY;

		double learningRate;
		int blurStrength;

        int minArea = MIN_AREA;
        int maxArea = MAX_AREA;

	public:
		MotionTrack(int _blurStrength, double _learningRate)
		{
			bgsubtract = new cv::BackgroundSubtractorMOG2( HISTORY, THRESHOLD, true );
			blurStrength = _blurStrength * 2 + 1;
			learningRate = _learningRate;
		}

		std::vector<Target> MergeMoments(std::vector<cv::Moments> moments);

		void UpdateFrame(cv::Mat newFrame);

		cv::Mat getResult() {return resultFrame.clone();}
		cv::Mat getBackground() {return background.clone();}
		cv::Mat getGoalFrame() {return goalFrame.clone();}

		unsigned getTargetX() {return targetX;}
		unsigned getTargetY() {return targetY;}
};

//Simple struct to contain targets to shoot at
struct Target
{
	private:
		unsigned x, y, w, h;
	
	public:
		Target(unsigned _x, unsigned _y, unsigned _w, unsigned _h) : x(_x), y(_y), w(_w), h(_h) {}

		unsigned getX() {return x;}
		unsigned getY() {return y;}
		unsigned getW() {return w;}
		unsigned getH() {return h;}
		unsigned getArea() {return w*h;}
};

#endif
