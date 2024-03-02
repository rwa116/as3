// GPIO module
// Part of the Hardware Abstraction Layer (HAL)
// Responsible for configuring GPIO pins used in other modules

#ifndef _GPIO_H_
#define _GPIO_H_

void GPIO_configPinForGPIO(int header, int pin);

void GPIO_configPinForInput(int linuxPin);

#endif