// Accelerometer Module
// Part of the Hardware Abstraction Layer (HAL). Responsible for managing
// Zen Cape accelerometer functionality through I2C1. P9_17 and P9_18

#ifndef _ACCELEROMETER_H_
#define _ACCELEROMETER_H_

struct accel_values {
    int valX;
    int valY;
    int valZ;
};

void Accelerometer_init(void);
void Accelerometer_cleanup(void);

struct accel_values Accelerometer_readValues(void);

#endif