#ifndef _ACCELEROMETER_HPP_
#define _ACCELEROMETER_HPP_

// #define ZENCAPE_RED 1

#include "hal/periodTimer.hpp"

typedef struct {
    short x;
    short y;
    short z;
} AccelerometerValue;

/**
 * This class is an interface to the accelerometer sensor. It is responsible for
 * reading the accelerometer data and determining if a drastic change has
 * occurred.
 */
class Accelerometer {
   public:
    Accelerometer(Period* period);
    /**
     * Initializes the accelerometer sensor.
     */
    void init();
    /**
     * Stops the accelerometer sensor and performs clean up.
     */
    void stop();

    AccelerometerValue getValue();
    /**
     * Determines if a drastic change has occurred in the x, y, or z axis.
     */
    bool isDrasticChangeX(AccelerometerValue output);
    bool isDrasticChangeY(AccelerometerValue output);
    bool isDrasticChangeZ(AccelerometerValue output);

   private:
    int i2cFileDesc;
    Period* period;
};

#endif
