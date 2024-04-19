#ifndef _AIR_DRUM_HPP_
#define _AIR_DRUM_HPP_

#include "generateBeat.hpp"
#include "hal/accelerometer.hpp"

/**
 * Handles the air drumming functionality by reading the accelerometer data and
 * adding the beat to the queue if the threshold is crossed.
 */
class AirDrum {
   public:
    // use the other style of constructor
    AirDrum(Accelerometer* accelerometer, GenerateBeat* generateBeat)
        : accelerometer(accelerometer), generateBeat(generateBeat) {}
    void init(void);
    void stop(void);

   private:
    Accelerometer* accelerometer;
    GenerateBeat* generateBeat;
    std::thread airDrumThread;
    bool isRunning;
};

#endif
