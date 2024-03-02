#include "hal/pins.h"
#include <stdlib.h>
#include <stdio.h>

static void runCommand(char* command);

void Pins_configPinForGPIO(int header, int pin) {
    char buffer[64];
    sprintf(buffer, "config-pin p%d.%d gpio", header, pin);
    runCommand(buffer);
}

void Pins_configPinForI2C(int header, int pin) {
    char buffer[64];
    sprintf(buffer, "config-pin p%d.%d i2c", header, pin);
    runCommand(buffer);
}

void Pins_configPinDirection(int linuxPin, bool isInput) {
    char buffer[64];
    if(isInput) {
        sprintf(buffer, "echo in > /sys/class/gpio/gpio%d/direction", linuxPin);
    }
    else {
        sprintf(buffer, "echo out > /sys/class/gpio/gpio%d/direction", linuxPin);
    }
    runCommand(buffer);
}

static void runCommand(char* command) {
    FILE *pipe = popen(command, "r");
    char buffer[1024];

    while (!feof(pipe) && !ferror(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL)
        break;
    }

    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != 0) {
        printf("Unable to execute command: %s, exit code: %d\n", command, exitCode);
        exit(1);
    }
}