#include "motiontrack.hpp"
#include <stdio.h>
#include <map>
#include <cmath>

float distance(int x1, int x2, int y1, int y2, float area1, float area2);
void handleGrouping(std::vector<std::vector<unsigned> > &neighbors, std::map<unsigned, bool> &handled, unsigned i);

void MotionTrack::UpdateFrame(cv::Mat newFrame)
{
	cv::Mat tmpFrame = newFrame.clone();
	goalFrame = tmpFrame.clone();
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

	moments = {};

	if (hierarchy.size() > 0)
	{
		for (int i = 0; i >= 0; i = hierarchy[i][0])
		{
			cv::Moments moment = cv::moments((cv::Mat)contours[i]);
			double area = moment.m00;
			if ( area >= minArea && area <= maxArea )
			{
				moments.push_back(moment);
			}
		}
	}
	targets = MergeMoments(moments);

	for (unsigned i = 0; i < targets.size(); i++)
	{
		cv::circle(tmpFrame, cv::Point(targets[i].getX(), targets[i].getY()), 60, cv::Scalar(255,255), 2);
	}

	Target primary(0,0,0,0);
	for (unsigned i = 0; i < targets.size(); i++)
	{
		if (targets[i].getArea() > primary.getArea())
		{
			primary = targets[i];
		}
	}
	targetX = primary.getX();
	targetY = primary.getY();

	resultFrame = tmpFrame.clone();
	
	cv::circle(goalFrame, cv::Point(primary.getX(), primary.getY()), 20, cv::Scalar(0,0,255), 2);
	cv::rectangle(goalFrame, cv::Point(primary.getX() - primary.getW()/2, primary.getY() - primary.getH()/2), cv::Point(primary.getX() + primary.getW()/2, primary.getY() + primary.getH()/2), cv::Scalar(0,0,255), 2, 8);
}

std::vector<Target> MotionTrack::MergeMoments(std::vector<cv::Moments> moments)
{
	unsigned x1, x2, y1, y2;
	float area1, area2;

	std::vector<std::vector<unsigned> > neighbors;
	std::vector<std::vector<cv::Moments> > groups;
	std::map<unsigned, int> to_handle = {};
	std::vector<Target> _targets = {};

	for (unsigned i = 0; i < moments.size(); i++)
	{
		auto m = moments[i];
		x1 = m.m10/m.m00;
		y1 = m.m01/m.m00;
		area1 = m.m00;

		std::vector<unsigned> v = {};
		neighbors.push_back(v);

		for (unsigned j = 0; j <= moments.size(); j++)
		{
			if (i != j)
			{
				auto n = moments[j];
				x2 = n.m10/n.m00;
				y2 = n.m01/n.m00;
				area2 = n.m00;
				if (distance(x1, y1, x2, y2, area1, area2) < MOMENT_MERGE_THRESH)
				{
					neighbors[i].push_back(j);
				}
			}
		}
	}

	for (unsigned i = 0; i < moments.size(); i++)
	{
		to_handle[i] = -1;
	}

	for (unsigned i = 0; i < moments.size(); i++)
	{
		if (to_handle.find(i) != to_handle.end())
		{
			int val = to_handle.find(i)->second;
			if (val == -1)
			{
				std::vector<cv::Moments> v = {moments[i]};
				val = groups.size();

				groups.push_back(v);
			}
			else
			{
				groups[val].push_back(moments[i]);
			}

			for (unsigned j = 0; j < neighbors[i].size(); j++)
			{
				if (to_handle.find(neighbors[i][j]) != to_handle.end())
				{
					to_handle[neighbors[i][j]] = val;
				}
			}

			to_handle.erase(i);
		}
	}

	for (unsigned i = 0; i < groups.size(); i++)
	{
		if (groups[i].size() > 0)
		{
			cv::Moments example = groups[i][0];
			int min_x = example.m10/example.m00, max_x = min_x, min_y = example.m01/example.m00, max_y = min_y;

			for (unsigned j = 0; j <= groups[i].size(); j++)
			{
				cv::Moments m = groups[i][j];
				int _x = m.m10/m.m00;
				int _y = m.m01/m.m00;
				if (_x < min_x)
				{min_x = _x;}
				if (_x > max_x)
				{max_x = _x;}
				if (_y < min_y)
				{min_y = _y;}
				if (_y > max_y)
				{max_y = _y;}
			}

			int x = (min_x + max_x);
			int y = (min_y + max_y);
			int w = max_x - min_x + MOMENT_MERGE_THRESH;
			int h = max_y - min_y + MOMENT_MERGE_THRESH;
			Target t(x, y, w, h);
			_targets.push_back(t);
		}
	}
	return _targets;
}

float distance(int x1, int y1, int x2, int y2, float area1, float area2)
{
	int _x, _y;
	if (x1 < x2)
	{_x = (x2 - sqrt(area2)) - (x1 + sqrt(area1));}
	else
	{_x = (x1 - sqrt(area1)) - (x2 + sqrt(area2));}

	if (y1 < y2)
	{_y = (y2 - sqrt(area2)) - (y1 + sqrt(area1));}
	else
	{_y = (y1 - sqrt(area1)) - (y2 + sqrt(area2));}

	return sqrt(pow(_x, 2) + pow(_y, 2));
}

