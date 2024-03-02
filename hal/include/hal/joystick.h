// Joystick module
// Part of the Hardware Abstraction Layer (HAL)
// Responsible for managing BeagleBone joystick input
// Contains functions that read the values of the gpio pins that are responsible for joystick input

#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

enum joy_direction {
    UP,
    RIGHT,
    DOWN,
    LEFT,
    NO_DIRECTION
};

void Joystick_init(void);
void Joystick_cleanup(void);

enum joy_direction Joystick_getPressedDirection(void);

#endif