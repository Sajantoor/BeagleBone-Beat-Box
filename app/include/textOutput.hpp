#ifndef _TEXT_OUPUT_HPP_
#define _TEXT_OUPUT_HPP_

#include <thread>

#include "generateBeat.hpp"
#include "hal/periodTimer.hpp"

/**
 * This class is responsible for printing the statistics of the sampler to the
 * console.
 */
class TextOutput {
   private:
    std::thread textoutputThread;
    bool isRunningOutput = true;
    void start(void);

    GenerateBeat* beatGenerator;
    Period* period;
    Socket* socket;

   public:
    TextOutput(GenerateBeat* generateBeat, Period* period, Socket* socket);
    void stop(void);
};

#endif
