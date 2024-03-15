#ifndef _AIR_DRUM_HPP_
#define _AIR_DRUM_HPP_

#include "generateBeat.hpp"
#include "hal/accelerometer.hpp"

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
