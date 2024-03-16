#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>

#include "beatGenerator.h"
#include "hal/audioMixer.h"

#define BEAT_DIR "beatbox-wav-files/"
#define BASE_DRUM_FILE BEAT_DIR "100051__menegass__gui-drum-bd-hard.wav"
#define HI_HAT_FILE BEAT_DIR "100053__menegass__gui-drum-cc.wav"
#define SNARE_FILE BEAT_DIR "100059__menegass__gui-drum-snare-soft.wav"

#define BPM_MIN 40
#define BPM_MAX 300

static _Atomic bool isRunning = true;
static _Atomic int bpm = 120;
static _Atomic int beat = 1;
static wavedata_t baseDrumData;
static wavedata_t hiHatData;
static wavedata_t snareData;

static void *beatThread(void *arg);
static pthread_t beatThreadId;
static pthread_mutex_t volumeLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t tempoLock = PTHREAD_MUTEX_INITIALIZER;

enum beatNum {
    BEAT_1,
    BEAT_1_5,
    BEAT_2,
    BEAT_2_5,
    BEAT_3,
    BEAT_3_5,
    BEAT_4,
    BEAT_4_5,
    NUM_BEATS,
};

static void queueNextBeat(enum beatNum bNum);

static void sleepForMs(long long delayInMs);

void BeatGenerator_init(void) {
    // Read wav files into our wavedata_t variables
    AudioMixer_readWaveFileIntoMemory(BASE_DRUM_FILE, &baseDrumData);
    AudioMixer_readWaveFileIntoMemory(HI_HAT_FILE, &hiHatData);
    AudioMixer_readWaveFileIntoMemory(SNARE_FILE, &snareData);
    pthread_create(&beatThreadId, NULL, beatThread, NULL);
}

void BeatGenerator_cleanup(void) {
    isRunning = false;
    pthread_join(beatThreadId, NULL);
    AudioMixer_freeWaveFileData(&baseDrumData);
    AudioMixer_freeWaveFileData(&hiHatData);
    AudioMixer_freeWaveFileData(&snareData);
}

void BeatGenerator_requestAudio(enum AudioClip clipType) {
    switch(clipType) {
        case BASE_DRUM:
            AudioMixer_queueSound(&baseDrumData);
            break;
        case HI_HAT:
            AudioMixer_queueSound(&hiHatData);
            break;
        case SNARE:
            AudioMixer_queueSound(&snareData);
            break;
        default:
            printf("Invalid audio clip requested.\n");
            break;
    }
}

int BeatGenerator_getBpm(void) {
    int fetchedBpm;
    pthread_mutex_lock(&tempoLock);
    fetchedBpm = bpm;
    pthread_mutex_unlock(&tempoLock);
    return fetchedBpm;
}

void BeatGenerator_incrementBpm(void) {
    pthread_mutex_lock(&tempoLock);
    bpm = bpm >= 295 ? 300 : bpm + 5;
    pthread_mutex_unlock(&tempoLock);
}

void BeatGenerator_decrementBpm(void) {
    pthread_mutex_lock(&tempoLock);
    bpm = bpm <= 45 ? 40 : bpm - 5;
    pthread_mutex_unlock(&tempoLock);
}


int BeatGenerator_getVolume(void) {
    pthread_mutex_lock(&volumeLock);
    int currentVol = AudioMixer_getVolume();
    pthread_mutex_unlock(&volumeLock);
    return currentVol;
}

void BeatGenerator_incrementVolume(void) {
    pthread_mutex_lock(&volumeLock);
    int currentVol = AudioMixer_getVolume();
    AudioMixer_setVolume(currentVol >= 95 ? 100 : currentVol + 5);
    pthread_mutex_unlock(&volumeLock);
}

void BeatGenerator_decrementVolume(void) {
    pthread_mutex_lock(&volumeLock);
    int currentVol = AudioMixer_getVolume();
    AudioMixer_setVolume(currentVol <= 5 ? 0 : currentVol - 5);
    pthread_mutex_unlock(&volumeLock);
}

int BeatGenerator_getBeat(void) {
    return beat;
}

void BeatGenerator_setBeat(int newBeat) {
    if (newBeat < 0 || newBeat > 2) {
        printf("ERROPR: Beat must be between 0 and 2.\n");
        return;
    }
    beat = newBeat;
}

static void *beatThread(void *arg) {
    (void)arg;
    int msToSleep;
    while(isRunning) {
        //printf("Current BPM = %d, current volume = %d\n", bpm, AudioMixer_getVolume());
        for(enum beatNum currBeatNum = 0; currBeatNum < NUM_BEATS; currBeatNum++) {
            queueNextBeat(currBeatNum);
            msToSleep = 60000 / bpm / 2;
            sleepForMs(msToSleep);
        }
    }
    return NULL;
}

// Plays the halfBeatNum'th beat according to the currently selected beat/mode
static void queueNextBeat(enum beatNum bNum) {
    switch(beat) {
        case 0:
            break;
        case 1:
            switch(bNum) {
                case BEAT_1:
                case BEAT_3:
                    AudioMixer_queueSound(&baseDrumData);
                    AudioMixer_queueSound(&hiHatData);
                    break;
                case BEAT_1_5:
                case BEAT_3_5: 
                    AudioMixer_queueSound(&hiHatData);
                    break;
                case BEAT_2:
                case BEAT_4: 
                    AudioMixer_queueSound(&hiHatData);
                    AudioMixer_queueSound(&snareData);
                    break;
                case BEAT_2_5:
                case BEAT_4_5: 
                    AudioMixer_queueSound(&hiHatData);
                    break;
                default:
                    break;
            }
            break;
        case 2:
            switch(bNum) {
                case BEAT_1:
                case BEAT_3:
                    AudioMixer_queueSound(&snareData);
                    break;
                case BEAT_1_5:
                case BEAT_3_5: 
                    AudioMixer_queueSound(&snareData);
                    break;
                case BEAT_2:
                case BEAT_4: 
                    AudioMixer_queueSound(&baseDrumData);
                    break;
                case BEAT_2_5:
                case BEAT_4_5: 
                    AudioMixer_queueSound(&baseDrumData);
                    break;
                default:
                    break;

            }
            break;
        default:
            printf("Error: invalid beat select, only 0, 1, 2 are valid.\n");
            sleepForMs(100);
            break;
    }
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