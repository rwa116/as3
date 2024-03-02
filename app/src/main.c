#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "hal/audioMixer.h"

#define BEAT_DIR "beatbox-wav-files/"
#define BASE_DRUM_FILE BEAT_DIR "100051__menegass__gui-drum-bd-hard.wav"
#define HI_HAT_FILE BEAT_DIR "100053__menegass__gui-drum-cc.wav"
#define SNARE_FILE BEAT_DIR "100059__menegass__gui-drum-snare-soft.wav"

static void sleepForMs(long long delayInMs);

int main() {
    wavedata_t drumSound;
    wavedata_t snareSound;
    
    AudioMixer_init();

    AudioMixer_readWaveFileIntoMemory(BASE_DRUM_FILE, &drumSound);
    AudioMixer_readWaveFileIntoMemory(SNARE_FILE, &snareSound);
    while(true) {
        AudioMixer_queueSound(&drumSound);
        sleepForMs(50);
        AudioMixer_queueSound(&snareSound);
        sleepForMs(50);
        AudioMixer_queueSound(&snareSound);
        sleepForMs(1000);
    }

    AudioMixer_cleanup();

    return 0;
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