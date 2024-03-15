#include <common/utils.hpp>
#include <iostream>
#include <vector>

#include "airDrum.hpp"
#include "controller.hpp"
#include "generateBeat.hpp"
#include "hal/accelerometer.hpp"
#include "hal/audioMixer.hpp"
#include "hal/joystick.hpp"
#include "hal/periodTimer.hpp"
#include "messageHandler.hpp"
#include "shutdown.hpp"
#include "socket.hpp"
#include "textOutput.hpp"

int main(void) {
    Period period;

    JoyStick joystick;
    joystick.init();

    Accelerometer accelerometer(&period);
    accelerometer.init();

    AudioMixer audioMixer(&period);
    audioMixer.init();

    GenerateBeat generateBeat(&audioMixer);
    generateBeat.init();

    Controller controller(&generateBeat, &joystick, &accelerometer);
    controller.init();

    AirDrum airDrum(&accelerometer, &generateBeat);
    airDrum.init();

    TextOutput textOutput(&generateBeat, &period);

    Socket socket;

    Shutdown shutdown(&accelerometer, &audioMixer, &generateBeat, &controller,
                      &airDrum, &textOutput);

    MessageHandler messageHandler(&socket, &generateBeat, &shutdown);
    messageHandler.init();

    return 0;
}
