#include "hardware.hpp"

void GPIOState::run(bool *keepRunning, std::mutex *hardWareMutex, double* mainX, double* mainY)
{
	//thread's main loop
	
	std::chrono::duration<double, std::milli> sleep_duration(200);
	
	while(*keepRunning) {
		//fetches the coordinates from the main thread
		hardWareMutex->lock();
		double targetX = *mainX;
		double targetY = *mainY;
		hardWareMutex->unlock();

        if (targetX >= 0)
        {
            targetX = targetX / maxWidth * 100;
            targetY = targetY / maxHeight * 100;

            targetX = calibrationLowerX + (targetX/100 * (calibrationUpperX - calibrationLowerX));
            targetY = calibrationLowerY + (targetY/100 * (calibrationUpperY - calibrationLowerY));
        }
        else
        {
            targetX = 50;
            targetY = 50;
        }
        setServoPosition(XSERVO, (int)targetX);
		setServoPosition(YSERVO, (int)targetY);
		
		std::this_thread::sleep_for(sleep_duration);
	}
}
