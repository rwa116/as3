// State Reader Module
// Responsible for maintaining a thread that reads the state of the joystick
// and accelerometer using their respictive modules in the HAL layer

#ifndef _STATE_READER_H_
#define _STATE_READER_H_

#include "hal/joystick.h"

void StateReader_init(void);
void StateReader_cleanup(void);

enum joy_direction StateReader_getJoystickValue(void);

#endif