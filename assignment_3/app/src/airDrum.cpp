#include "airDrum.hpp"

#include <thread>

#include "common/utils.hpp"

const int DEBOUNCE_TIME = 100;

void AirDrum::init(void) {
    isRunning = true;
    airDrumThread = std::thread([this] {
        // for debouncing x, y, z
        int xActivatedTimer = 0;
        int yActivatedTimer = 0;
        int zActivatedTimer = 0;

        while (isRunning) {
            AccelerometerValue value = accelerometer->getValue();
            if (accelerometer->isDrasticChangeX(value) &&
                xActivatedTimer == 0) {
                xActivatedTimer = DEBOUNCE_TIME;
                generateBeat->playSnare();
            }

            if (accelerometer->isDrasticChangeY(value) &&
                yActivatedTimer == 0) {
                yActivatedTimer = DEBOUNCE_TIME;
                generateBeat->playHiHat();
            }

            if (accelerometer->isDrasticChangeZ(value) &&
                zActivatedTimer == 0) {
                zActivatedTimer = DEBOUNCE_TIME;
                generateBeat->playBaseDrum();
            }

            // for debouncing x, y, z
            if (xActivatedTimer > 0) {
                xActivatedTimer--;
            }

            if (yActivatedTimer > 0) {
                yActivatedTimer--;
            }

            if (zActivatedTimer > 0) {
                zActivatedTimer--;
            }

            sleepForMs(1);
        }
    });
}

void AirDrum::stop(void) {
    isRunning = false;
    airDrumThread.join();
}
