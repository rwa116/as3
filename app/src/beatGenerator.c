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
    return bpm;
}

void BeatGenerator_setBpm(int newBpm) {
    if (newBpm < BPM_MIN || newBpm > BPM_MAX) {
		printf("ERROR: BPM must be between 40 and 300.\n");
		return;
	}
    bpm = newBpm;
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
    while(isRunning) {
        //printf("Current BPM = %d, current volume = %d\n", bpm, AudioMixer_getVolume());
        switch(beat) {
            case 0: // Off
                sleepForMs(100);
                break;
            case 1: // Standard Beat 
            {
                for(int i=0; i<2; i++) {
                    int msToSleep = 60000 / bpm / 2;
                    // Beat 1 / 3
                    AudioMixer_queueSound(&baseDrumData);
                    AudioMixer_queueSound(&hiHatData);
                    sleepForMs(msToSleep);
                    // Beat 1.5 / 3.5
                    AudioMixer_queueSound(&hiHatData);
                    sleepForMs(msToSleep);
                    // Beat 2 / 4
                    AudioMixer_queueSound(&hiHatData);
                    AudioMixer_queueSound(&snareData);
                    sleepForMs(msToSleep);
                    // Beat 2.5 / 4.5
                    AudioMixer_queueSound(&hiHatData);
                    sleepForMs(msToSleep);
                }
                break;
            }
            case 2: // New beat
            {
                for(int i=0; i<2; i++) {
                    int msToSleep = 60000 / bpm / 2;
                    // Beat 1 / 3
                    AudioMixer_queueSound(&snareData);
                    sleepForMs(msToSleep);
                    // Beat 1.5 / 3.5
                    AudioMixer_queueSound(&snareData);
                    sleepForMs(msToSleep);
                    // Beat 2 / 4
                    AudioMixer_queueSound(&baseDrumData);
                    sleepForMs(msToSleep);
                    // Beat 2.5 / 4.5
                    AudioMixer_queueSound(&baseDrumData);
                    sleepForMs(msToSleep);
                }
                break;
            }
            default:
                printf("Error: invalid beat select, only 0, 1, 2 are valid.\n");
                sleepForMs(1000);
                break;
        }
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