#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "hal/periodTimer.h"
#include "hal/accelerometer.h"
#include "hal/pins.h"

#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"

#define I2C_DEVICE_ADDRESS 0x18

#define CTRL_REG1 0x20
#define OUT_X_L 0x28
#define OUT_Y_L 0x2A
#define OUT_Z_L 0x2C


static _Atomic int i2cFileDesc;

static int initI2cBus(char* bus, int address);
static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value);
static void readI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char *buff, int length);

#define HEADER_9 9
void Accelerometer_init(void) {
    Pins_configPinForI2C(HEADER_9, 17);
    Pins_configPinForI2C(HEADER_9, 18);
    initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    writeI2cReg(i2cFileDesc, CTRL_REG1, 0x27); // Power mode 1, enable X, Y, Z
}

void Accelerometer_cleanup(void) {
    close(i2cFileDesc);
}

#define REG_LSB 0
#define REG_MSB 1
#define REG_YLSB 2
#define REG_YMSB 3
#define REG_ZLSB 4
#define REG_ZMSB 5
struct accel_values Accelerometer_readValues(void) {
    unsigned char buff[6];
    struct accel_values fetchedVals;
    //unsigned char regAddr = OUT_X_L + 0x80; // Add 0x80 to enable auto-increment
    Period_markEvent(PERIOD_EVENT_SAMPLE_ACCEL);

    readI2cReg(i2cFileDesc, OUT_X_L + 0x80, buff, 6);
    int16_t rawFetched = (buff[REG_MSB] << 8) | (buff[REG_LSB]);
    if(((rawFetched >> 15) & 1) == 1) {
        rawFetched = -((rawFetched ^ 0xFFFF) + 1);
    }
    fetchedVals.valX = rawFetched;

    // readI2cReg(i2cFileDesc, OUT_Y_L + 0x80, buff, 2);
    rawFetched = (buff[REG_YMSB] << 8) | (buff[REG_YLSB]);
    if(((rawFetched >> 15) & 1) == 1) {
        rawFetched = -((rawFetched ^ 0xFFFF) + 1);
    }
    fetchedVals.valY = rawFetched;

    // readI2cReg(i2cFileDesc, OUT_Z_L + 0x80, buff, 2);
    rawFetched = (buff[REG_ZMSB] << 8) | (buff[REG_ZLSB]);
    if(((rawFetched >> 15) & 1) == 1) {
        rawFetched = -((rawFetched ^ 0xFFFF) + 1);
    }
    fetchedVals.valZ = rawFetched;

    return fetchedVals;
}


static int initI2cBus(char* bus, int address)
{
	i2cFileDesc = open(bus, O_RDWR);
	if (i2cFileDesc < 0) {
		printf("I2C DRV: Unable to open bus for read/write (%s)\n", bus);
		perror("Error is:");
		exit(-1);
	}

	int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
	if (result < 0) {
		perror("Unable to set I2C device to slave address.");
		exit(-1);
	}
	return i2cFileDesc;
}

static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value)
{
	unsigned char buff[2];
	buff[0] = regAddr;
	buff[1] = value;
	int res = write(i2cFileDesc, buff, 2);
	if (res != 2) {
		perror("Unable to write i2c register");
		exit(-1);
	}
}

static void readI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char *buff, int length)
{
	// To read a register, must first write the address
	int res = write(i2cFileDesc, &regAddr, sizeof(regAddr));
	if (res != sizeof(regAddr)) {
		perror("Unable to write i2c register.");
		exit(-1);
	}

	// Now read the value and return it
	res = read(i2cFileDesc, buff, length);
	if (res != length) {
		perror("Unable to read i2c register");
		exit(-1);
	}
}