#include "hardware.hpp"

void GPIOState::run(bool *keepRunning, std::mutex *hardWareMutex, double* mainX, double* mainY, bool *startShooting, bool *shootingDone)
{
	//thread's main loop


	//The loop will be executed approx five times a second
	std::chrono::duration<double, std::milli> sleep_duration(200);

	while(*keepRunning) {
		//fetches the coordinates from the main thread
		hardWareMutex->lock();
		double targetX = *mainX;
		double targetY = *mainY;
        _startShooting = *startShooting;
		hardWareMutex->unlock();

		//The following block handles the trigger mechanism

        //printf("%i : %i\n", (int)std::chrono::system_clock::now() - (int)servoActivationTime, (int)triggerServoDuration);

        if (triggerServoActive && std::chrono::system_clock::now() - servoActivationTime > triggerServoDuration)
        {
            if (!_disableShooting)
            {
                setServoPosition(TRIGGERSERVO, calibrationLowerTrigger);
            }
            triggerServoActive = false;
            hardWareMutex->lock();
            *shootingDone = true;
            hardWareMutex->unlock();
        }

        if (!triggerServoActive && _startShooting)
        {
            if (!_disableShooting)
            {
                setServoPosition(TRIGGERSERVO, calibrationUpperTrigger);
            }
            servoActivationTime = std::chrono::system_clock::now();
            triggerServoActive = true;
            hardWareMutex->lock();
            *startShooting = false;
            hardWareMutex->unlock();
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
