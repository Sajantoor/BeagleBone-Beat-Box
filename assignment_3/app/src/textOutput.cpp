/*
Each second, prints the following statistics to the terminal.

Line 1:
- # light samples taken during the previous second
- Raw value from the POT, and how many hertz (Hz) this is for the PWM
- The averaged light level (from exponential smoothing), displayed as a voltage
with 3 decimal places.
- The number of light level dips that have been found in samples from the
previous second
- Timing jitter information (provided by periodTimer.h/.c) for samples collected
during the previous second Minimum time between light samples. Maximum time
between light samples. Average time between light samples. Number of times
sampled

        Format:
        Smpl ms[{min}, {max}] avg {avg}/{num-samples}

Line 2:
- Display 20 sample from the previous second.
- These values must be as evenly spaced across the collected samples as possible
(for example, if you have ~100 samples collected, display every ~5th sample). If
there are less than 20 samples from the previous second, display all the values.

Format: {sample number}:{value}
*/

#include "textOutput.hpp"

#include <iomanip>
#include <iostream>
#include <vector>

#include "common/utils.hpp"
#include "hal/periodTimer.hpp"

static constexpr const int PRECISION = 3;
static constexpr const int NUM_SAMPLES_TO_PRINT = 1;

TextOutput::TextOutput(GenerateBeat* generateBeat, Period* period,
                       Socket* socket) {
    this->beatGenerator = generateBeat;
    this->period = period;
    this->socket = socket;
    start();
}

void TextOutput::start(void) {
    std::cout << "Starting to sample data..." << std::endl;
    textoutputThread = std::thread([this] {
        while (this->isRunningOutput) {
            BEAT_TYPE mode = beatGenerator->getBeatType();
            unsigned int bpm = beatGenerator->getTempo();
            int volume = beatGenerator->getVolume();
            Period_statistics_t audioStats;
            period->getStatisticsAndClear(PERIOD_EVENT_AUDIO, &audioStats);

            Period_statistics_t accelStats;
            period->getStatisticsAndClear(PERIOD_EVENT_ACCELEROMETER,
                                          &accelStats);

            // Sample output: M0 90bpm vol:80 Audio[16.283, 16.942]
            // avg 16.667/61 Accel[12.276, 13.965] avg 12.998/77

            std::cout << "M" << mode << " " << bpm << "bpm vol:" << volume
                      << " Audio[" << std::fixed << std::setprecision(PRECISION)
                      << audioStats.minPeriodInMs << ", "
                      << audioStats.maxPeriodInMs << "] avg "
                      << audioStats.avgPeriodInMs << "/"
                      << audioStats.numSamples << " Accel[" << std::fixed
                      << std::setprecision(PRECISION)
                      << accelStats.minPeriodInMs << ", "
                      << accelStats.maxPeriodInMs << "] avg "
                      << accelStats.avgPeriodInMs << "/"
                      << accelStats.numSamples << std::endl;

            socket->sendMessageToWebServer("all mode " + std::to_string(mode) +
                                           " bpm " + std::to_string(bpm) +
                                           " volume " + std::to_string(volume));

            std::cout << std::endl;

            sleepForMs(1000);
        }
    });
}

void TextOutput::stop(void) {
    this->isRunningOutput = false;
    textoutputThread.join();
}
