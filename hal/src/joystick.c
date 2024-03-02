#include "hal/joystick.h"
#include "hal/pins.h"
#include <stdlib.h>
#include <stdio.h>

static void readFromFile(char* filename, char* buff);
static void getValue(enum joy_direction direction, char* buff);

struct direction_info {
    enum joy_direction direction;
    const int pinNumber;
    char* directionValueDirectory;
};

#define GPIO_DIRECTORY "/sys/class/gpio/"
// Data on the joystick directions that are used in this program
static const struct direction_info directionInfo[] = {
    {UP, 26, GPIO_DIRECTORY "gpio26/value"},
    {RIGHT, 47, GPIO_DIRECTORY "gpio47/value"},
    {DOWN, 46, GPIO_DIRECTORY "gpio46/value"},
    {LEFT, 65, GPIO_DIRECTORY "gpio65/value"},
    {IN, 27, GPIO_DIRECTORY "gpio27/value"},
};

#define HEADER_8 8
void Joystick_init(void) {
    // Configure pins for GPIO
    Pins_configPinForGPIO(HEADER_8, 14);
    Pins_configPinForGPIO(HEADER_8, 15);
    Pins_configPinForGPIO(HEADER_8, 16);
    Pins_configPinForGPIO(HEADER_8, 17);
    Pins_configPinForGPIO(HEADER_8, 18);

    // Configure pins for Input
    Pins_configPinDirection(directionInfo[UP].pinNumber, true);
    Pins_configPinDirection(directionInfo[RIGHT].pinNumber, true);
    Pins_configPinDirection(directionInfo[DOWN].pinNumber, true);
    Pins_configPinDirection(directionInfo[LEFT].pinNumber, true);
    Pins_configPinDirection(directionInfo[IN].pinNumber, true);
}

void Joystick_cleanup(void) {

}

enum joy_direction Joystick_getPressedDirection(void) {
    char* pressedBuffer = malloc(1024);
    for(enum joy_direction direction = UP; direction <= IN; direction++) { // iterate through all 4 directions
        getValue(direction, pressedBuffer);
        if(pressedBuffer[0] == '0') {
            return direction;
        }
    }
    return NO_DIRECTION;
}

static void getValue(enum joy_direction direction, char* buff) {
    readFromFile(directionInfo[direction].directionValueDirectory, buff);
}

static void readFromFile(char* filename, char* buff) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("ERROR: Unable to open file (%s) for read\n", filename);
        exit(-1);
    }

    const int MAX_LENGTH = 1024;
    fgets(buff, MAX_LENGTH, file);

    fclose(file);
}