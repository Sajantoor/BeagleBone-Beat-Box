#include "hal/i2c.hpp"

// This methods are adapted from I2C Guide
// https://opencoursehub.cs.sfu.ca/bfraser/grav-cms/cmpt433/guides/files/I2CGuide.pdf

int I2C::initBus(const char* bus, int address) {
    int i2cFileDesc = open(bus, O_RDWR);
    int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
    if (result < 0) {
        perror("I2C: Unable to set I2C device to slave address.");
        exit(1);
    }
    return i2cFileDesc;
}

void I2C::writeRegister(int i2cFileDesc, unsigned char regAddr,
                        unsigned char value) {
    unsigned char buff[2];
    buff[0] = regAddr;
    buff[1] = value;
    int res = write(i2cFileDesc, buff, 2);
    if (res != 2) {
        perror("I2C: Unable to write i2c register.");
        exit(1);
    }
}

unsigned char I2C::readRegister(int i2cFileDesc, unsigned char regAddr) {
    // To read a register, must first write the address
    int res = write(i2cFileDesc, &regAddr, sizeof(regAddr));
    if (res != sizeof(regAddr)) {
        perror("I2C: Unable to write to i2c register.");
        exit(1);
    }
    // Now read the value and return it
    char value = 0;
    res = read(i2cFileDesc, &value, sizeof(value));
    if (res != sizeof(value)) {
        perror("I2C: Unable to read from i2c register");
        exit(1);
    }
    return value;
}

void I2C::readNRegisters(int i2cFileDesc, unsigned char regAddr,
                         unsigned char* buffer, size_t n) {
    // To read a register, must first write the address
    size_t res = write(i2cFileDesc, &regAddr, sizeof(regAddr));
    if (res != sizeof(regAddr)) {
        perror("I2C: Unable to write to i2c register.");
        exit(1);
    }
    // Now read the value and return it
    res = read(i2cFileDesc, buffer, n);
    size_t nBytes = n * sizeof(unsigned char);

    if (res != nBytes) {
        perror("I2C: Unable to read from i2c register");
        exit(1);
    }
}
