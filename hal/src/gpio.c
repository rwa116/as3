#include "hal/gpio.h"
#include <stdlib.h>
#include <stdio.h>

static void runCommand(char* command);

void GPIO_configPinForGPIO(int header, int pin) {
    char buffer[64];
    sprintf(buffer, "config-pin p%d.%d gpio", header, pin);
    runCommand(buffer);
}

void GPIO_configPinForInput(int linuxPin) {
    char buffer[64];
    sprintf(buffer, "echo in > /sys/class/gpio/gpio%d/direction", linuxPin);
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