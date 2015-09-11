#ifndef _HARDWARE_HPP
#define _HARDWARE_HPP

#include <fstream>
#include <string>
#include <mutex>

const unsigned SERVOAMOUNT = 8;
enum {XSERVO, YSERVO, TRIGGERSERVO};

class GPIOState
{
private:
	int servoPositions[8];
	std::ofstream file;
	float calibrationShiftX;
	float calibrationShiftY;
	float maxWidth;
	float maxHeight;
public:
	//calibration shift refers to the angle between the camera and
	//the nerf gun

	//constructor initializes every servo at their centre point
	//it also prompts for data about calibrational shifts.
	GPIOState(float _calibrationShiftX, float _calibrationShiftY
		, float _maxWidth, float _maxHeight) {
		for(unsigned i=0;i<8;i++) {
			servoPositions[i] = 50;
		}
		file.open("/dev/servoblaster");
		calibrationShiftX = _calibrationShiftX;
		calibrationShiftY = _calibrationShiftY;
		maxWidth = _maxWidth;
		maxHeight = _maxHeight;
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
	void runThread(bool *keepRunning, std::mutex *hardWareMutex, float *mainX, float *mainY);
};



#endif
