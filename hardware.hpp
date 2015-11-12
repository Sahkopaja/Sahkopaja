#ifndef _HARDWARE_HPP
#define _HARDWARE_HPP

#include <fstream>
#include <string>
#include <mutex>
#include <thread>
#include <chrono>

#include "preferences.hpp"

const unsigned SERVOAMOUNT = 8;
enum {XSERVO, YSERVO, TRIGGERSERVO};

class GPIOState
{
private:
	int servoPositions[SERVOAMOUNT];
	std::ofstream file;
	double calibrationLowerX;
	double calibrationUpperX;
	double calibrationLowerY;
	double calibrationUpperY;

	double maxWidth;
	double maxHeight;
public:
	//calibration shift refers to the angle between the camera and
	//the nerf gun

	//constructor initializes every servo at their centre point
	//it also prompts for data about calibrational shifts.
	GPIOState(Preferences *pref)
	{
		for(unsigned i = 0; i < SERVOAMOUNT; i++) {
			servoPositions[i] = 50;
		}
		file.open("/dev/servoblaster");

		calibrationLowerX = pref->readInt("servo_calibration_lower_x", 0);
		calibrationUpperX = pref->readInt("servo_calibration_upper_x", 0);
		calibrationLowerY = pref->readInt("servo_calibration_lower_y", 0);
		calibrationUpperY = pref->readInt("servo_calibration_upper_y", 0);
		maxWidth = pref->readInt("camera_frameW", 240);
		maxHeight = pref->readInt("camera_frameH", 180);
	}
	//destructor closes the file
	~GPIOState() {
		if(file.is_open()){
			file.close();
		}
	}
	//Returns true if stream is open, false if it is not open
	bool getStreamState() {
		return file.is_open();
	}
	//sets the position of a servo number <servoNumber>. Position varies
	//from 0 to 100.
	bool setServoPosition(unsigned servoNumber, int percentage) {
		if(file.is_open()) {
			file << servoNumber << "=" << percentage << "%\n";
			return true;
		}
		return false;
	}
	//gets a position of a servo. returns -1 if index requested is too
	//large.
	int getPosition(unsigned servoNumber) {
		if(servoNumber >= SERVOAMOUNT) {
			return -1;
		}
		return servoPositions[servoNumber];
	}
	void run(bool *keepRunning, std::mutex *hardWareMutex, double *mainX, double *mainY);
	std::thread runThread(bool *keepRunning, std::mutex *hardWareMutex, double *mainX, double *mainY)
	{
		return std::thread([=] { run(keepRunning, hardWareMutex, mainX, mainY); } );
	}
};

#endif
