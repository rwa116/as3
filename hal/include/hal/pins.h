// Pins module
// Part of the Hardware Abstraction Layer (HAL)
// Responsible for configuring pins used in other modules

#ifndef _PINS_H_
#define _PINS_H_

#include <stdbool.h>

void Pins_configPinForGPIO(int header, int pin);
void Pins_configPinForI2C(int header, int pin);

void Pins_configPinDirection(int linuxPin, bool isInput);

#endif