// Beat Generator Module
// Responsible for orchestrating playback of available sounds upon request
// and maintaining selected drum beat at current npm

#ifndef BEAT_GENERATOR_H
#define BEAT_GENERATOR_H

enum AudioClip {
    BASE_DRUM,
    HI_HAT,
    SNARE,
    NUM_CLIPS
};

void BeatGenerator_init(void);
void BeatGenerator_cleanup(void);

void BeatGenerator_requestAudio(enum AudioClip clipType);

int BeatGenerator_getBpm(void);
void BeatGenerator_incrementBpm(void);
void BeatGenerator_decrementBpm(void);

int BeatGenerator_getVolume(void);
void BeatGenerator_incrementVolume(void);
void BeatGenerator_decrementVolume(void);

int BeatGenerator_getBeat(void);
void BeatGenerator_setBeat(int newBeat);

#endif