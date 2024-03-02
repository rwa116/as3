#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

#include "stateReader.h"
#include "beatGenerator.h"
#include "hal/joystick.h"
#include "hal/audioMixer.h"
#include "hal/accelerometer.h"

// static long long getTimeInMs(void);
static void sleepForMs(long long delayInMs);

static _Atomic bool isRunning = true;
static _Atomic enum joy_direction pressedDirection;

static pthread_t stateThreadId;
static void *stateThread(void *arg);

void StateReader_init(void) {
    pthread_create(&stateThreadId, NULL, stateThread, NULL);
}

void StateReader_cleanup(void) {
    isRunning = false;
    pthread_join(stateThreadId, NULL);
}

enum joy_direction StateReader_getJoystickValue(void) {
    return pressedDirection;
}

#define GRAVITY 16384
static void *stateThread(void *arg) {
    (void)arg;

    while(isRunning) {
        pressedDirection = Joystick_getPressedDirection();
        switch(pressedDirection) {
            case UP:
            {
                int currentVolume = AudioMixer_getVolume();
                AudioMixer_setVolume(currentVolume >= 95 ? 100 : currentVolume + 5);
                sleepForMs(200);
                break;
            }
            case DOWN:
            {
                int currentVolume = AudioMixer_getVolume();
                AudioMixer_setVolume(currentVolume <= 5 ? 0 : currentVolume - 5);
                sleepForMs(200);
                break;
            }
            case LEFT:
            {
                int currentBpm = BeatGenerator_getBpm();
                BeatGenerator_setBpm(currentBpm <= 45 ? 40 : currentBpm - 5);
                sleepForMs(200);
                break;
            }
            case RIGHT:
            {
                int currentBpm = BeatGenerator_getBpm();
                BeatGenerator_setBpm(currentBpm >= 295 ? 300 : currentBpm + 5);
                sleepForMs(200);
                break;
            }
            case IN:
            {
                int currentBeat = BeatGenerator_getBeat();
                BeatGenerator_setBeat(currentBeat >= 2 ? 0 : currentBeat + 1);
                sleepForMs(200);
                break;
            }
            default:
                break;
        }
        #define THRESHOLD 16000
        struct accel_values accelValues = Accelerometer_readValues();
        if((accelValues.valX > THRESHOLD) || (accelValues.valX < -THRESHOLD)) {
            BeatGenerator_requestAudio(SNARE);
                sleepForMs(200);
        }
        if((accelValues.valY > THRESHOLD) || (accelValues.valY < -THRESHOLD)) {
            BeatGenerator_requestAudio(BASE_DRUM);
                sleepForMs(200);
        }
        if((accelValues.valZ > THRESHOLD + GRAVITY) || (accelValues.valZ < -THRESHOLD + GRAVITY)) {
            BeatGenerator_requestAudio(HI_HAT);
                sleepForMs(200);
        }

        sleepForMs(10);
    }

    return NULL;
}

static void sleepForMs(long long delayInMs) {
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;
    long long delayNs = delayInMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}

// static long long getTimeInMs(void) {
//     struct timespec spec;
//     clock_gettime(CLOCK_REALTIME, &spec);
//     long long seconds = spec.tv_sec;
//     long long nanoSeconds = spec.tv_nsec;
//     long long milliSeconds = seconds * 1000
//     + nanoSeconds / 1000000;
//     return milliSeconds;
// }