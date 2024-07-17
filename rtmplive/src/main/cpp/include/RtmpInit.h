//
// Created by MMM on 2024/7/10.
//

#ifndef MYYFFMPEG_RTMPINIT_H
#define MYYFFMPEG_RTMPINIT_H

#define LOG_TAG "wy"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#include <android/log.h>
#include <thread>
#include "PacketQueue.h"

using namespace std;

typedef void (*RtmpStatusCallback)(void *, const char *, float codeErr);

typedef void (*GetAudioTagCallback)(void *);

extern "C"
{
#include "rtmp.h"
}


class RtmpInit {

private:
    const char *mUrl = nullptr;
    thread *childThread = nullptr;
    void *mContext = nullptr;
    uint32_t start_time;
    std::atomic<bool> isPushing;
    PacketQueue<RTMPPacket *> packetQueue;


public:
    RtmpStatusCallback mStatusCallback = nullptr;
    GetAudioTagCallback mGetAudioTagCallback = nullptr;

private:
    static void DoRtmpInit(RtmpInit *rtmpInit);

    void callbackStatusMsg(const char *msg, float codeErr);

    void startThread();

    void releasePackets(RTMPPacket *&packet);

public:
    void startRtmp(const char *path);

    void addRtmpPacket(RTMPPacket *packet);

    void stopRtmp();

    void pauseRtmp();

    void releaseRtmp();

    void setRtmpStatusCallback(void *context, RtmpStatusCallback callback);

    void setAudioTagCallBack(void *context, GetAudioTagCallback mGetAudioTagCallback);

    void getAudioTag();

    ~RtmpInit();
};


#endif //MYYFFMPEG_RTMPINIT_H
