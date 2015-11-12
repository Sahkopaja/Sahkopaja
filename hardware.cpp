#include "hardware.hpp"

void GPIOState::run(bool *keepRunning, std::mutex *hardWareMutex, double* mainX, double* mainY)
{
	//thread's main loop
	
	std::chrono::duration<double, std::milli> sleep_duration(5);
	
	while(*keepRunning) {
		double targetX = 0.0f;
		double targetY = 0.0f;
		//fetches the coordinates from the main thread
		hardWareMutex->lock();
		targetX = *mainX;
		targetY = *mainY;
		hardWareMutex->unlock();
		
		double finalX = calibrationLowerX + (targetX/100 * (calibrationUpperX - calibrationLowerX));
		double finalY = calibrationLowerY + (targetY/100 * (calibrationUpperY - calibrationLowerY));

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
		
		std::this_thread::sleep_for(sleep_duration);
	}
}
