#ifndef _ACCELEROMETER_HPP_
#define _ACCELEROMETER_HPP_

// #define ZENCAPE_RED 1

typedef struct {
    short x;
    short y;
    short z;
} AccelerometerOutput;

class Accelerometer {
   public:
    void init();
    void stop();
    AccelerometerOutput getValue();

   private:
    int i2cFileDesc;
};

#endif
