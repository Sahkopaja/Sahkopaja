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
	int calibrationLowerX;
	int calibrationUpperX;
	int calibrationLowerY;
	int calibrationUpperY;

	int calibrationUpperTrigger;
	int calibrationLowerTrigger;
	int triggerDuration;

	int maxWidth;
	int maxHeight;

	std::chrono::duration<int, std::milli> triggerServoDuration;
	std::chrono::system_clock::time_point servoActivationTime;
	bool triggerServoActive;

public:

	//constructor initializes every servo at their centre point
	//it also prompts for data about calibrational shifts.
	GPIOState(Preferences *pref)
	{
		file.open("/dev/servoblaster");

		//Load all the required preferences
		calibrationLowerX = pref->readInt("servo_calibration_lower_x", 0);
		calibrationUpperX = pref->readInt("servo_calibration_upper_x", 100);
		calibrationLowerY = pref->readInt("servo_calibration_lower_y", 0);
		calibrationUpperY = pref->readInt("servo_calibration_upper_y", 100);
		calibrationLowerTrigger = pref->readInt("servo_calibration_lower_trigger", 0);
		calibrationUpperTrigger = pref->readInt("servo_calibration_upper_trigger", 100);
		triggerDuration = pref->readInt("servo_calibration_trigger_duration", 2000);
		maxWidth = pref->readInt("camera_frameW", 240);
		maxHeight = pref->readInt("camera_frameH", 180);

		triggerServoDuration = std::chrono::duration<int, std::milli>(triggerDuration);
		triggerServoActive = false;

		servoPositions[XSERVO] = (calibrationLowerX + calibrationUpperX) / 2;
		servoPositions[YSERVO] = (calibrationLowerY + calibrationUpperY) / 2;
		servoPositions[TRIGGERSERVO] = (calibrationLowerTrigger + calibrationUpperTrigger) / 2;
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

            //Corrections based on the servo being moved
            if (servoNumber == XSERVO)
            {
                percentage = 100 - percentage;
            }
            else if (servoNumber == YSERVO)
            {
                percentage = 100 - percentage;
            }
            else if (servoNumber == TRIGGERSERVO)
            {
                //Nothing ATM
            }

			file << servoNumber << "=" << percentage << "%" << std::endl;
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
	void run(bool *keepRunning, std::mutex *hardWareMutex, double *mainX, double *mainY, bool *targetConfirmed, bool *disableShooting);
	std::thread runThread(bool *keepRunning, std::mutex *hardWareMutex, double *mainX, double *mainY, bool *targetConfirmed, bool *disableShooting)
	{
		return std::thread([=] { run(keepRunning, hardWareMutex, mainX, mainY, targetConfirmed, disableShooting); } );
	}
};

#endif
