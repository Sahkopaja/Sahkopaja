#include "hardware.hpp"

void GPIOState::run(bool *keepRunning, std::mutex *hardWareMutex, double* mainX, double* mainY, bool *targetConfirmed, bool *disableShooting)
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

		//The following block handle the trigger mechanism
		if (!disableShooting)
		{
			if (triggerServoActive && std::chrono::system_clock::now() - servoActivationTime > triggerServoDuration)
			{
				setServoPosition(TRIGGERSERVO, calibrationLowerTrigger);
				triggerServoActive = false;
			}

			if (!triggerServoActive && targetConfirmed)
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
            targetX = 50;
            targetY = 50;
        }
        setServoPosition(XSERVO, (int)targetX);
		setServoPosition(YSERVO, (int)targetY);

		std::this_thread::sleep_for(sleep_duration);
	}
}
