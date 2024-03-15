#ifndef _SHUTDOWN_HPP_
#define _SHUTDOWN_HPP_

#include <stdbool.h>

#include "airDrum.hpp"
#include "controller.hpp"
#include "generateBeat.hpp"
#include "hal/accelerometer.hpp"
#include "hal/audioMixer.hpp"
#include "textOutput.hpp"

/**
 * Stops the program and shuts down the system. Handles the graceful shutdown of
 * hardware components and threads.
 */
class Shutdown {
   private:
    bool shutdown = false;
    Accelerometer* accelerometer;
    AudioMixer* audioMixer;
    GenerateBeat* generateBeat;
    Controller* controller;
    AirDrum* airDrum;
    TextOutput* textOutput;

   public:
    Shutdown(Accelerometer* accelerometer, AudioMixer* audioMixer,
             GenerateBeat* generateBeat, Controller* controller,
             AirDrum* airDrum, TextOutput* textOutput)
        : accelerometer(accelerometer),
          audioMixer(audioMixer),
          generateBeat(generateBeat),
          controller(controller),
          airDrum(airDrum),
          textOutput(textOutput) {}
    /**
     * Checks if the system is shutdown
     */
    bool isShutdown(void);
    /**
     * Performs the shutdown by stopping all hardware components and
     * stopping all threads. Sets the shutdown flag to true.
     */
    void performShutdown(void);
};
#endif
