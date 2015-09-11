#include "hardware.hpp"

void GPIOState::runThread(bool *keepRunning, std::mutex *hardWareMutex, float* mainX, float* mainY)
{
	//thread's main loop
	while(*keepRunning) {
		float targetX = 0.0f;
		float targetY = 0.0f;
		//fetches the coordinates from the main thread
		hardWareMutex->lock();
		targetX = *mainX;
		targetY = *mainY;
		hardWareMutex->unlock();
		
		float finalX = targetX+calibrationShiftX;
		float finalY = targetY+calibrationShiftY;

		if(finalX > maxWidth) {
			finalX = maxWidth;
		}
		if(finalX < 0) {
			finalX = 0.0f;
		}
		if(finalY > maxHeight) {
			finalY = maxHeight;
		}
		if(finalY < 0.0f) {
			finalY = 0.0f;
		}
		finalX = 100.0f*finalX/maxWidth;
		finalY = 100.0f*finalY/maxHeight;
		setServoPosition(XSERVO, (int)finalX);
		setServoPosition(YSERVO, (int)finalY);
	}
}
