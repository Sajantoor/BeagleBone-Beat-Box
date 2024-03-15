#include "shutdown.hpp"

bool Shutdown::isShutdown(void) { return this->shutdown; }

void Shutdown::performShutdown(void) {
    // shutdown everything in reverse order than how it was initialized
    textOutput->stop();
    airDrum->stop();
    controller->stop();
    generateBeat->stop();
    audioMixer->stop();
    accelerometer->stop();

    this->shutdown = true;
}
