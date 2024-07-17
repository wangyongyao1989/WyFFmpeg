//
// Created by MMM on 2024/7/17.
//

#ifndef MYYFFMPEG_AUDIOSTREAMPACKET_H
#define MYYFFMPEG_AUDIOSTREAMPACKET_H

#define LOG_TAG "wy"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#include <android/log.h>

#include "rtmp.h"
#include "faac/faac.h"
#include <sys/types.h>
#include <string.h>


class AudioStreamPacket {

    typedef void (*AudioCallback)(void *, RTMPPacket *packet);

    typedef void (*RtmpStatusCallback)(void *, const char *, float codeErr);

private:
    AudioCallback mAudioCallback = nullptr;
    RtmpStatusCallback mStatusCallback = nullptr;
    void *mContext = nullptr;
    int m_channels = 0;
    faacEncHandle m_audioCodec = 0;
    u_long m_inputSamples;
    u_long m_maxOutputBytes;
    u_char *m_buffer = 0;

    void callbackStatusMsg(const char *msg, float codeErr);

public:

    AudioStreamPacket();

    ~AudioStreamPacket();

    int setAudioEncInfo(int samplesInHZ, int channels);

    int getInputSamples() const;

    void encodeData(int8_t *data);

    RTMPPacket *getAudioTag();

    void setAudioCallback(void *context, AudioCallback audioCallback);

    void setRtmpStatusCallback(void *context, RtmpStatusCallback callback);
};

#endif //MYYFFMPEG_AUDIOSTREAMPACKET_H
