//
// Created by MMM on 2024/7/10.
//

#include "include/RtmpInit.h"

using namespace std;

void RtmpInit::startRtmp() {
    if (childThread == nullptr) {
        childThread = new thread(DoAVdecoding, this);
    }
    childThread->detach();
}

void RtmpInit::stopRtmp() {
    if (mContext != nullptr && mStatusCallback != nullptr)
        mStatusCallback(mContext, "Rtmp stopRtmp", 0);

}

void RtmpInit::pauseRtmp() {
    if (mContext != nullptr && mStatusCallback != nullptr)
        mStatusCallback(mContext, "Rtmp pauseRtmp", 0);

}

void RtmpInit::releaseRtmp() {
    if (mContext != nullptr && mStatusCallback != nullptr)
        mStatusCallback(mContext, "Rtmp releaseRtmp", 0);

}

void RtmpInit::DoAVdecoding(RtmpInit *rtmpInit) {
    rtmpInit->startThread();
}

void RtmpInit::startThread() {
    if (mContext != nullptr && mStatusCallback != nullptr)
        mStatusCallback(mContext, "Rtmp startThread", 0);

}

void RtmpInit::setRtmpStatusCallback(void *context, RtmpStatusCallback callback) {
    mContext = context;
    mStatusCallback = callback;
}


