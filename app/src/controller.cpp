#include "controller.hpp"
#include <common/utils.hpp>

Controller::Controller(GenerateBeat* generateBeat, JoyStick* joystick, Accelerometer* accelerometer) {
    this->generateBeat = generateBeat;
    this->joystick = joystick;
    this->accelerometer = accelerometer;
}

void Controller::init(void) {
    isRunning = true;
    controllerThread = std::thread([this] {
        while (isRunning) {
            // Get the joystick input
            bool joystickActivated = true;
            const JoystickDirection dir = joystick->getDirection();
            switch (dir) {
                case UP:
                    generateBeat->increaseVolume();
                    break;
                case DOWN:
                    generateBeat->decreaseVolume();
                    break;
                case LEFT:
                    generateBeat->decreaseTempo();
                    break;
                case RIGHT:
                    generateBeat->increaseTempo();
                    break;
                case BUTTON:
                    generateBeat->cycleBeatType();
                    break;
                default:
                    joystickActivated = false;
                    break;
            }
            if (joystickActivated) sleepForMs(250);
        }
    });
}

void Controller::stop(void) {
    isRunning = false;
    controllerThread.join();
}