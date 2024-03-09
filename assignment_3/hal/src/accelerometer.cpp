#include "hal/accelerometer.hpp"

#include <iostream>

#include "hal/gpio.hpp"
#include "hal/i2c.hpp"

#ifndef ZENCAPE_RED
#define I2C_ADDRESS 0x1c
#define CTRL_REG1 0x2A

#define REG_STATUS 0x00
#define OUT_X_MSB_REG 0x01
#define OUT_X_LSB_REG 0x02
#define OUT_Y_MSB_REG 0x03
#define OUT_Y_LSB_REG 0x04
#define OUT_Z_MSB_REG 0x05
#define OUT_Z_LSB_REG 0x06
#else
#define I2C_ADDRESS 0x18
#define CTRL_REG1 0x20

#define REG_STATUS 0x27
#define OUT_X_MSB_REG 0x29
#define OUT_X_LSB_REG 0x28
#define OUT_Y_MSB_REG 0x2B
#define OUT_Y_LSB_REG 0x2A
#define OUT_Z_MSB_REG 0x2D
#define OUT_Z_LSB_REG 0x2C
#endif

#define BUFFER_SIZE 7
static const char* I2C_BUS = "/dev/i2c-1";

void Accelerometer::init() {
    GPIO::configPin(P9, 17, "i2c");
    GPIO::configPin(P9, 18, "i2c");
    i2cFileDesc = I2C::initBus(I2C_BUS, I2C_ADDRESS);

// Write 1 bit to the CTRL_REG1 register to set the accelerometer to active
// mode
#ifndef ZENCAPE_RED
    I2C::writeRegister(i2cFileDesc, CTRL_REG1, 0x01);
#else
    // On zencape red the ctrl register looks like this:
    // PM2 PM1 PM0 DR1 DR0 Zen Yen Xen
    // Power is in PM2 - PM0 bits
    // Active mode is 0b001 in PM2 - PM0 bits
    IC2::writeRegister(i2cFileDesc, CTRL_REG1, 1 << 5);
#endif
}

AccelerometerOutput Accelerometer::getValue() {
    // Read the X, Y, and Z values from the accelerometer
    // MSBs are 8 MSBs of the 12 bit sample, LSBs are the 4 LSBs of the 12 bit
    // sample
    unsigned char buffer[BUFFER_SIZE];

    /*
    Need to read 7 bytes to get the 6 bytes for the X, Y, and Z values (need
    to read the status byte as well): Reasoning for this:

    T_X_MSB, OUT_X_LSB, OUT_Y_MSB, OUT_Y_LSB, OUT_Z_MSB, and OUT_Z_LSB are
    stored in the auto-incrementing address range of 0x01 to 0x06 to reduce
    reading the status followed by 12-bit axis data to 7 bytes. If the F_READ
    bit is set (0x2A bit 1), auto-increment will skip over LSB registers. This
    will shorten the data acquisition from 7 bytes to 4 bytes. The LSB registers
    can only be read immediately following the read access of the corresponding
    MSB register. A random read access to the LSB registers is not possible.
    Reading the MSB register and then the LSB register in sequence ensures
    that both bytes (LSB and MSB) belong to the same data sample, even if a
    new data sample arrives between reading the MSB and the LSB byte.
    */
    I2C::readNRegisters(i2cFileDesc, REG_STATUS, buffer, BUFFER_SIZE);
#ifndef ZENCAPE_RED
    unsigned char xMSB = buffer[1];
    unsigned char xLSB = buffer[2];
    unsigned char yMSB = buffer[3];
    unsigned char yLSB = buffer[4];
    unsigned char zMSB = buffer[5];
    unsigned char zLSB = buffer[6];

#else
    unsigned char xMSB = buffer[2];
    unsigned char xLSB = buffer[1];
    unsigned char yMSB = buffer[4];
    unsigned char yLSB = buffer[3];
    unsigned char zMSB = buffer[6];
    unsigned char zLSB = buffer[5];
#endif
    // // Combine the MSB and LSB values to get the final value
    short x = (xMSB << 8) | xLSB;
    short y = (yMSB << 8) | yLSB;
    short z = (zMSB << 8) | zLSB;

    return {x, y, z};
}

void Accelerometer::stop() {
    // set accelerator to inactive
    I2C::writeRegister(i2cFileDesc, CTRL_REG1, 0x00);
    // config pin back to default
    GPIO::configPin(P9, 17, "gpio");
    GPIO::configPin(P9, 18, "gpio");
    close(i2cFileDesc);
}