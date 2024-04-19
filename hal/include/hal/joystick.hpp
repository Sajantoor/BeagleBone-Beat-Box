#ifndef _JOYSTICK_HPP_
#define _JOYSTICK_HPP_

#define JOYSTICK_UP_PIN 26
#define JOYSTICK_DOWN_PIN 46
#define JOYSTICK_LEFT_PIN 65
#define JOYSTICK_RIGHT_PIN 47
#define JOYSTICK_BUTTON_PIN 27

typedef enum { BUTTON, UP, DOWN, LEFT, RIGHT, NO_INPUT } JoystickDirection;

/**
 * This class is an interface to the joystick. Checks if the joystick is pressed
 * and in which direction it is pressed.
 */
class JoyStick {
   public:
    void init();
    /**
     * Returns the direction the joystick is pressed in.
     */
    JoystickDirection getDirection();
    /**
     * Returns true if the joystick is pressed.
     */
    bool isButtonPressed();

   private:
    bool isPressed(JoystickDirection dir);
};

#endif
