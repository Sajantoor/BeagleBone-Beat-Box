#ifndef __CONTROLLER_HPP__
#define __CONTROLLER_HPP__

#include <thread>

#include "generateBeat.hpp"
#include "hal/accelerometer.hpp"
#include "hal/joystick.hpp"

/**
 * Generates a beat depending on the input from the joystick and accelerometer
 */
class Controller {
   private:
    bool isRunning;
    std::thread controllerThread;
    GenerateBeat* generateBeat;
    JoyStick* joystick;
    Accelerometer* accelerometer;

   public:
    Controller(GenerateBeat* generateBeat, JoyStick* joystick,
               Accelerometer* accelerometer);
    void init(void);
    void stop(void);
};

#endif