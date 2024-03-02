#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "hal/audioMixer.h"
#include "hal/joystick.h"
#include "hal/accelerometer.h"
#include "hal/periodTimer.h"
#include "beatGenerator.h"
#include "stateReader.h"
#include "statistics.h"

// static void sleepForMs(long long delayInMs) {
//     const long long NS_PER_MS = 1000 * 1000;
//     const long long NS_PER_SECOND = 1000000000;
//     long long delayNs = delayInMs * NS_PER_MS;
//     int seconds = delayNs / NS_PER_SECOND;
//     int nanoseconds = delayNs % NS_PER_SECOND;
//     struct timespec reqDelay = {seconds, nanoseconds};
//     nanosleep(&reqDelay, (struct timespec *) NULL);
// }

int main() {
    
    Period_init();
    AudioMixer_init();
    Joystick_init();
    Accelerometer_init();
    BeatGenerator_init();
    StateReader_init();
    Statistics_init();
    
    while(1) {
        // BeatGenerator_requestAudio(SNARE);
        // sleepForMs(250);
        // BeatGenerator_requestAudio(HI_HAT);
        // sleepForMs(250);
        // BeatGenerator_requestAudio(HI_HAT);
    }

    Statistics_cleanup();
    StateReader_cleanup();
    BeatGenerator_cleanup();
    Accelerometer_cleanup();
    Joystick_cleanup();
    AudioMixer_cleanup();
    Period_cleanup();

    return 0;
}