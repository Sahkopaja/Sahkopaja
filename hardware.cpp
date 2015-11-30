#include "hardware.hpp"

void GPIOState::run(bool *keepRunning, std::mutex *hardWareMutex, double* mainX, double* mainY, bool *startShooting, bool *shootingDone, bool *disableShooting)
{
	//thread's main loop


	//The loop will be executed approx five times a second
	std::chrono::duration<double, std::milli> sleep_duration(200);

	while(*keepRunning) {
		//fetches the coordinates from the main thread
		hardWareMutex->lock();
		double targetX = *mainX;
		double targetY = *mainY;
		hardWareMutex->unlock();

		//The following block handles the trigger mechanism
		if (!disableShooting)
		{
			if (triggerServoActive && std::chrono::system_clock::now() - servoActivationTime > triggerServoDuration)
			{
				setServoPosition(TRIGGERSERVO, calibrationLowerTrigger);
				triggerServoActive = false;
				hardWareMutex->lock();
				*shootingDone = true;
				hardWareMutex->unlock();
			}

			if (!triggerServoActive && startShooting)
			{
				setServoPosition(TRIGGERSERVO, calibrationUpperTrigger);
				servoActivationTime = std::chrono::system_clock::now();
				triggerServoActive = true;
			}
		}

		//Calculate percentage values to x and y servos to based on screen coordinates
        if (targetX >= 0)
        {
            targetX = targetX / maxWidth * 100;
            targetY = targetY / maxHeight * 100;

            targetX = calibrationLowerX + (targetX/100 * (calibrationUpperX - calibrationLowerX));
            targetY = calibrationLowerY + (targetY/100 * (calibrationUpperY - calibrationLowerY));
        }
        else
        {
            targetX = (calibrationUpperX + calibrationLowerX) / 2;
            targetY = (calibrationUpperX + calibrationLowerX) / 2;
        }
        setServoPosition(XSERVO, (int)targetX);
		setServoPosition(YSERVO, (int)targetY);

		std::this_thread::sleep_for(sleep_duration);
	}
}
