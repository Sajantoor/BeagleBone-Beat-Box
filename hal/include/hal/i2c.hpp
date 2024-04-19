#ifndef _I2C_HPP_
#define _I2C_HPP_

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

/**
 * Contains methods to interact with I2C devices.
 */
class I2C {
   public:
    static int initBus(const char* bus, int address);
    static void writeRegister(int i2cFileDesc, unsigned char regAddr,
                              unsigned char value);
    static unsigned char readRegister(int i2cFileDesc, unsigned char regAddr);
    static void readNRegisters(int i2cFileDesc, unsigned char regAddr,
                               unsigned char* buffer, size_t n);
};

#endif