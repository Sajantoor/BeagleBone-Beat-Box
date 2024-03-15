#ifndef _TEXT_OUPUT_HPP_
#define _TEXT_OUPUT_HPP_

#include <thread>

#include "generateBeat.hpp"
#include "hal/periodTimer.hpp"

/*
This module is responsible for printing the statistics of the sampler to the
console.
*/
class TextOutput {
   private:
    std::thread statisticsThread;
    bool isRunningOutput = true;
    void start(void);

    GenerateBeat* beatGenerator;
    Period* period;

   public:
    TextOutput(GenerateBeat* generateBeat, Period* period);
    void stop(void);
};

#endif
