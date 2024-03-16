#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

#include "stateReader.h"
#include "beatGenerator.h"
#include "hal/joystick.h"
#include "hal/audioMixer.h"
#include "hal/accelerometer.h"

static long long getTimeInMs(void);
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

    long long lastTimePressedUp = 0;
    long long lastTimePressedDown = 0;
    long long lastTimePressedLeft = 0;
    long long lastTimePressedRight = 0;
    long long lastTimePressedIn = 0;

    long long lastTimeAccelX = 0;
    long long lastTimeAccelY = 0;
    long long lastTimeAccelZ = 0;

    while(isRunning) {
        pressedDirection = Joystick_getPressedDirection();
        long long currentTime = getTimeInMs();
        switch(pressedDirection) {
            case UP:
                if(currentTime - lastTimePressedUp > 300) {
                    BeatGenerator_incrementVolume();
                    lastTimePressedUp = currentTime;
                }
                break;
            case DOWN:
                if(currentTime - lastTimePressedDown > 300) {
                    BeatGenerator_decrementVolume();
                    lastTimePressedDown = currentTime;
                }
                break;
            case LEFT:
                if(currentTime - lastTimePressedLeft > 300) {
                    BeatGenerator_decrementBpm();
                    lastTimePressedLeft = currentTime;
                }
                break;
            case RIGHT:
                if(currentTime - lastTimePressedRight > 300) {
                    BeatGenerator_incrementBpm();
                    lastTimePressedRight = currentTime;
                }
                break;
            case IN:
                if(currentTime - lastTimePressedIn > 300) {
                    int currentBeat = BeatGenerator_getBeat();
                    BeatGenerator_setBeat(currentBeat >= 2 ? 0 : currentBeat + 1);
                    lastTimePressedIn = currentTime;
                }
                break;
            default:
                break;
        }
        #define THRESHOLD 20000
        #define Z_POS_THRESHOLD 32000
        #define Z_NEG_THRESHOLD 12000
        struct accel_values accelValues = Accelerometer_readValues();
        if(((accelValues.valX > THRESHOLD) || (accelValues.valX < -THRESHOLD)) 
            && (currentTime - lastTimeAccelX > 250)) {
            BeatGenerator_requestAudio(SNARE);
            lastTimeAccelX = currentTime;
        }
        else if(((accelValues.valY > THRESHOLD) || (accelValues.valY < -THRESHOLD)) 
            && (currentTime - lastTimeAccelY > 250)) {
            BeatGenerator_requestAudio(BASE_DRUM);
            lastTimeAccelY = currentTime;
        }
        else if(((accelValues.valZ > Z_POS_THRESHOLD) || (accelValues.valZ < -THRESHOLD)) 
            && (currentTime - lastTimeAccelZ > 250)) {
            BeatGenerator_requestAudio(HI_HAT);
            lastTimeAccelZ = currentTime;
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

static long long getTimeInMs(void) {
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    long long seconds = spec.tv_sec;
    long long nanoSeconds = spec.tv_nsec;
    long long milliSeconds = seconds * 1000
    + nanoSeconds / 1000000;
    return milliSeconds;
}