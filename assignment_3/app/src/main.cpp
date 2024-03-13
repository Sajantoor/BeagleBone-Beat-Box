#include <iostream>
#include <vector>

#include <common/utils.hpp>
#include "generateBeat.hpp"
#include "hal/accelerometer.hpp"
#include "hal/audioMixer.hpp"
#include "hal/joystick.hpp"
#include "messageHandler.hpp"
#include "socket.hpp"
#include "controller.hpp"

int main(void) {
    JoyStick joystick;
    joystick.init();
    Accelerometer accelerometer;
    accelerometer.init();
    AudioMixer audioMixer;
    audioMixer.init();
    GenerateBeat generateBeat(&audioMixer);
	generateBeat.init();
	Controller controller(&generateBeat, &joystick, &accelerometer);
	controller.init();

    while (1) {
        accelerometer.getValue();
        sleepForMs(1000);
    }

	// TODO: shutdown via separate module upon UDP command
	controller.stop();
    accelerometer.stop();
    generateBeat.stop();
    audioMixer.stop();
    return 0;
}
