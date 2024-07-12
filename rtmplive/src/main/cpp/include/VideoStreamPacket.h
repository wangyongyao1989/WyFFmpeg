//
// Created by MMM on 2024/7/11.
//

#ifndef MYYFFMPEG_VIDEOSTREAMPACKET_H
#define MYYFFMPEG_VIDEOSTREAMPACKET_H

#include <mutex>
#define LOG_TAG "wy"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#include <android/log.h>
extern "C"
{
#include "rtmp.h"
#include "x264/x264.h"
}

class VideoStreamPacket {
    typedef void (*VideoCallback)(RTMPPacket *packet);
    typedef void (*RtmpStatusCallback)(void *, const char *, float codeErr);


private:
    std::mutex m_mutex;

    int m_frameLen;
    x264_t *videoCodec = 0;
    x264_picture_t *pic_in = 0;
    void *mContext;

    VideoCallback mVideoCallback;
    RtmpStatusCallback mStatusCallback = nullptr;

    void sendSpsPps(uint8_t *sps, uint8_t *pps, int sps_len, int pps_len);

    void sendFrame(int type, uint8_t *payload, int i_payload);

    void callbackStatusMsg(const char *msg, float codeErr);

public:
    VideoStreamPacket();

    ~VideoStreamPacket();

    int setVideoEncInfo(int width, int height, int fps, int bitrate);

    void encodeVideo(int8_t *data);

    void setVideoCallback(VideoCallback videoCallback);

    void setRtmpStatusCallback(void *context, RtmpStatusCallback callback);

};


#endif //MYYFFMPEG_VIDEOSTREAMPACKET_H
