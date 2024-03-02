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
void BeatGenerator_setBpm(int newBpm);
int BeatGenerator_getBeat(void);
void BeatGenerator_setBeat(int newBeat);

#endif