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

using namespace std;

typedef void (*RtmpStatusCallback)(void *, const char *, float codeErr);

extern "C"
{
#include "rtmp.h"

}


class RtmpInit {

private:
    const char *mUrl = nullptr;
    thread *childThread = nullptr;
    void *mContext = nullptr;
public:
    RtmpStatusCallback mStatusCallback = nullptr;

private:
    static void DoAVdecoding(RtmpInit *rtmpInit);

    void callbackMsg(const char *msg, float codeErr);

    void startThread();

public:
    void startRtmp(const char *path);

    void stopRtmp();

    void pauseRtmp();

    void releaseRtmp();

    void setRtmpStatusCallback(void *context, RtmpStatusCallback callback);

};


#endif //MYYFFMPEG_RTMPINIT_H
