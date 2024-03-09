#include <iostream>
#include <vector>

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
    }

    accelerometer.stop();
    return 0;
}
