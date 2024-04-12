//
// Created by yangw on 2018-3-6.
//

#ifndef MYMUSIC_WLQUEUE_H
#define MYMUSIC_WLQUEUE_H

#include "queue"
#include "pthread.h"

extern "C"
{
#include "libavutil/avutil.h"
#include "libavcodec/avcodec.h"
};


class MNQueue {

public:
    std::queue<AVPacket *> queuePacket;
    pthread_mutex_t mutexPacket;
    pthread_cond_t condPacket;
    bool  playStatus = false;

public:
    MNQueue();
    ~MNQueue();
    int push(AVPacket *packet);
    int get(AVPacket *packet);
    int size();
    void clearAvpacket();

};


#endif //MYMUSIC_WLQUEUE_H
