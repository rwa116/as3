#include <pthread.h>
#include <stdbool.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "shutdown.h"
#include "beatGenerator.h"

enum Command {
    MODE_0,
    MODE_1,
    MODE_2,
    VOL_UP,
    VOL_DOWN,
    TEMPO_UP,
    TEMPO_DOWN,
    PLAY_HI_HAT,
    PLAY_SNARE,
    PLAY_BASE,
    TERMINATE,
    UNKNOWN
};

static pthread_t networkThread;
static _Atomic bool isRunning;

static void *listenLoop(void *arg);
static enum Command checkCommand(char* input);
static void sendReply(enum Command command, int socketDescriptor, struct sockaddr_in *sinRemote);

void Network_init(void) {
    isRunning = true;
    pthread_create(&networkThread, NULL, listenLoop, NULL);
}

void Network_cleanup(void) {
    isRunning = false;
    pthread_join(networkThread, NULL);
}

#define MAX_LEN 1500
#define PORT 12345
static void *listenLoop(void *arg) {
    (void)arg;
    enum Command lastCommand = UNKNOWN;
    // Socket initialization
    struct sockaddr_in sin = {0};
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(PORT);

    int socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if(socketDescriptor == -1) {
        perror("Failed to establish server socket");
        exit(1);
    }
    bind(socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));

    // Message receive and reply loop
    while(!Shutdown_isShutdown()) {
        struct sockaddr_in sinRemote;
        unsigned int sinLen = sizeof(sinRemote);
        char messageRx[MAX_LEN];

        int bytesRx = recvfrom(socketDescriptor, messageRx, MAX_LEN - 1, 0, 
            (struct sockaddr*) &sinRemote, &sinLen);

        messageRx[bytesRx] = 0;
        enum Command sentCommand = checkCommand(messageRx);
        lastCommand = sentCommand;
        sendReply(lastCommand, socketDescriptor, &sinRemote);
    }

    close(socketDescriptor);

    return NULL;
}

static enum Command checkCommand(char* input) {
    if(strcmp(input, "mode 0") == 0) {
        return MODE_0;
    }
    else if(strcmp(input, "mode 1") == 0) {
        return MODE_1;
    }
    else if(strcmp(input, "mode 2") == 0) {
        return MODE_2;
    }
    else if(strcmp(input, "volup") == 0) {
        return VOL_UP;
    }
    else if(strcmp(input, "voldown") == 0) {
        return VOL_DOWN;
    }
    else if(strcmp(input, "tempoup") == 0) {
        return TEMPO_UP;
    }
    else if(strcmp(input, "tempodown") == 0) {
        return TEMPO_DOWN;
    }
    else if(strcmp(input, "playhihat") == 0) {
        return PLAY_HI_HAT;
    }
    else if(strcmp(input, "playsnare") == 0) {
        return PLAY_SNARE;
    }
    else if(strcmp(input, "playbase") == 0) {
        return PLAY_BASE;
    }
    else if(strcmp(input, "terminate") == 0) {
        return TERMINATE;
    }
    else {
        return UNKNOWN;
    }
}

#define MAX_SAMPLE_SIZE 8
static void sendReply(enum Command command, int socketDescriptor, struct sockaddr_in *sinRemote) {
    char messageTx[MAX_LEN];
    unsigned int sinLen;

    switch(command) {
        case MODE_0:
            BeatGenerator_setBeat(0);
            snprintf(messageTx, MAX_LEN, "modeid,None");
            break;
        case MODE_1:
            BeatGenerator_setBeat(1);
            snprintf(messageTx, MAX_LEN, "modeid,Rock #1");
            break;
        case MODE_2:
            BeatGenerator_setBeat(2);
            snprintf(messageTx, MAX_LEN, "modeid,Rock #2");
            break;
        case VOL_UP:
        {
            BeatGenerator_incrementVolume();
            int vol = BeatGenerator_getVolume();
            snprintf(messageTx, MAX_LEN, "volumeid,%d", vol);
            break;
        }
        case VOL_DOWN:
        {
            BeatGenerator_decrementVolume();
            int vol = BeatGenerator_getVolume();
            snprintf(messageTx, MAX_LEN, "volumeid,%d", vol);
            break;
        }
        case TEMPO_UP:
        {
            BeatGenerator_incrementBpm();
            int bpm = BeatGenerator_getBpm();
            snprintf(messageTx, MAX_LEN, "tempoid,%d", bpm);
            break;
        }
        case TEMPO_DOWN:
        {
            BeatGenerator_decrementBpm();
            int bpm = BeatGenerator_getBpm();
            snprintf(messageTx, MAX_LEN, "tempoid,%d", bpm);
            break;
        }
        case PLAY_HI_HAT:
            BeatGenerator_requestAudio(HI_HAT);
            snprintf(messageTx, MAX_LEN, "soundreq,hi-hat");
            break;
        case PLAY_SNARE:
            BeatGenerator_requestAudio(SNARE);
            snprintf(messageTx, MAX_LEN, "soundreq,snare");
            break;
        case PLAY_BASE:
            BeatGenerator_requestAudio(BASE_DRUM);
            snprintf(messageTx, MAX_LEN, "soundreq,base drum");
            break;
        case TERMINATE:
            Shutdown_signalShutdown();
            snprintf(messageTx, MAX_LEN, "terminate,");
            break;
        case UNKNOWN:
            snprintf(messageTx, MAX_LEN, "Unknown command.\n");
            break;
        default:
            perror("Invalid command");
            exit(1);
    }
    
    sinLen = sizeof(*sinRemote);
    sendto(socketDescriptor, messageTx, strlen(messageTx), 0, 
        (struct sockaddr*) sinRemote, sinLen);
}