#ifndef _GPIO_HPP_
#define _GPIO_HPP_

#include <string>

typedef enum { P8 = 8, P9 = 9 } headerType;

/**
 * Contains methods for interacting with the GPIO pins.
 */
class GPIO {
   public:
    static void exportPin(int pin);
    static void configPin(headerType header, int pin, std::string setting);
    static int getPinValue(int pin);
    static void setPinActiveLow(int pin, int activeLow);
    static bool isPinActive(int pin);
    static void setPinValue(int pin, int value);
    static void setPinDirection(int pin, std::string direction);
};

#endif
