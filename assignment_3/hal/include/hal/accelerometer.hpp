#ifndef _ACCELEROMETER_HPP_
#define _ACCELEROMETER_HPP_

// #define ZENCAPE_RED 1

#include "hal/periodTimer.hpp"

typedef struct {
    short x;
    short y;
    short z;
} AccelerometerValue;

class Accelerometer {
   public:
    Accelerometer(Period* period);
    void init();
    void stop();
    AccelerometerValue getValue();
    bool isDrasticChangeX(AccelerometerValue output);
    bool isDrasticChangeY(AccelerometerValue output);
    bool isDrasticChangeZ(AccelerometerValue output);

   private:
    int i2cFileDesc;
    Period* period;
};

#endif
