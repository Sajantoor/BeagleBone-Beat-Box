#include <iostream>
#include <vector>

#include <common/utils.hpp>
#include "hal/accelerometer.hpp"
#include "hal/joystick.hpp"
#include "messageHandler.hpp"
#include "socket.hpp"

int main(void) {
    JoyStick joystick;
    joystick.init();
    Accelerometer accelerometer;
    accelerometer.init();

    while (!joystick.isButtonPressed()) {
        accelerometer.getValue();
        sleepForMs(1000);
    }

    accelerometer.stop();
    return 0;
}
