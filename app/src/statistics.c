#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>

#include "statistics.h"
#include "beatGenerator.h"
#include "hal/audioMixer.h"
#include "hal/periodTimer.h"

static void *statisticsThread(void *arg);
static void printStatistics(void);
static void sleepForMs(long long delayInMs);

static _Atomic bool isRunning;
static pthread_t statisticsThreadId;

void Statistics_init(void) {
    isRunning = true;
    pthread_create(&statisticsThreadId, NULL, statisticsThread, NULL);
}

void Statistics_cleanup(void) {
    isRunning = false;
    pthread_join(statisticsThreadId, NULL);
}

static void *statisticsThread(void *arg) {
    (void)arg;
    while(isRunning) {
        sleepForMs(1000);
        printStatistics();
    }
    return NULL;
}

static void printStatistics(void) {
    Period_statistics_t audioStats;
    Period_statistics_t accelStats;
    int mode = BeatGenerator_getBeat();
    int bpm = BeatGenerator_getBpm();
    int vol = AudioMixer_getVolume();
    Period_getStatisticsAndClear(PERIOD_EVENT_SAMPLE_AUDIO, &audioStats);
    Period_getStatisticsAndClear(PERIOD_EVENT_SAMPLE_ACCEL, &accelStats);

    printf("M%d %dbpm vol:%d  Audio[%2.3f, %2.3f] avg %2.3f/%d Accel[%2.3f, %2.3f] avg %2.3f/%d\n",
        mode, bpm, vol, audioStats.minPeriodInMs, audioStats.maxPeriodInMs, audioStats.avgPeriodInMs, 
        audioStats.numSamples, accelStats.minPeriodInMs, accelStats.maxPeriodInMs, 
        accelStats.avgPeriodInMs, accelStats.numSamples);
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