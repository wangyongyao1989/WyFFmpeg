//
// Created by MMM on 2024/7/10.
//

#ifndef MYYFFMPEG_RTMPINIT_H
#define MYYFFMPEG_RTMPINIT_H

#include <thread>

using namespace std;

typedef void (*RtmpStatusCallback)(void *, const char *, float codeErr);

class RtmpInit {

private:
    thread *childThread = nullptr;
    void *mContext = nullptr;
public:
    RtmpStatusCallback mStatusCallback = nullptr;

private:
    static void DoAVdecoding(RtmpInit *rtmpInit);

    void startThread();

public:
    void startRtmp();
    void stopRtmp();
    void pauseRtmp();
    void releaseRtmp();
    void setRtmpStatusCallback(void *context,RtmpStatusCallback callback);

};


#endif //MYYFFMPEG_RTMPINIT_H
