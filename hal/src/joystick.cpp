#include "hal/joystick.hpp"

#include "hal/gpio.hpp"

void JoyStick::init() {
    // Config pin for each of the joystick directions and pressed down
    for (int pin : {14, 15, 16, 17, 18}) {
        GPIO::configPin(P8, pin, "gpio");
    }

    GPIO::setPinDirection(JOYSTICK_UP_PIN, "in");
    GPIO::setPinDirection(JOYSTICK_DOWN_PIN, "in");
    GPIO::setPinDirection(JOYSTICK_LEFT_PIN, "in");
    GPIO::setPinDirection(JOYSTICK_RIGHT_PIN, "in");
    GPIO::setPinDirection(JOYSTICK_BUTTON_PIN, "in");
}

/**
 * Returns true if the joystick is pressed, false otherwise.
 */
bool JoyStick::isButtonPressed() {
    // TODO: May need to debounce this but works fine now...
    return isPressed(BUTTON);
}

/**
 * Returns the joystick's direction
 */
JoystickDirection JoyStick::getDirection() {
    // Check all directions and return the first one that is pressed
    for (JoystickDirection dir : {UP, DOWN, LEFT, RIGHT, BUTTON}) {
        if (isPressed(dir)) {
            return dir;
        }
    }

    return NO_INPUT;
}

/**
 * Returns true if the joystick is pressed in a given direction, false
 * otherwise.
 */
bool JoyStick::isPressed(JoystickDirection dir) {
    int value;

    switch (dir) {
        case UP:
            value = GPIO::getPinValue(JOYSTICK_UP_PIN);
            break;
        case DOWN:
            value = GPIO::getPinValue(JOYSTICK_DOWN_PIN);
            break;
        case LEFT:
            value = GPIO::getPinValue(JOYSTICK_LEFT_PIN);
            break;
        case RIGHT:
            value = GPIO::getPinValue(JOYSTICK_RIGHT_PIN);
            break;
        case BUTTON:
            value = GPIO::getPinValue(JOYSTICK_BUTTON_PIN);
            break;
        default:
            printf("ERROR: Invalid direction.\n");
            // TODO: Handle this gracefully.
            exit(1);
    }

    // "Joystick pins read 0 when pressed and 1 when not pressed"
    return value == 0;
}
