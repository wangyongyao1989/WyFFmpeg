//
// Created by MMM on 2024/4/16.
//

#ifndef MYYFFMPEG_FFMPEGPLAYAUDIO_H
#define MYYFFMPEG_FFMPEGPLAYAUDIO_H

#include "ffmpegpch.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <assert.h>

// for native asset manager
#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>




class FFmpegPlayAudio {
private:
    uint8_t *outputBuffer;
    size_t outputBufferSize;

    AVPacket packet;
    int audioStream;
    AVFrame *aFrame;
    SwrContext *swr;
    AVFormatContext *aFormatCtx = nullptr;
    AVCodecContext *aCodecCtx;
    static SLEngineItf engineEngine;
    static SLPlayItf bqPlayerPlay;
    static SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
    static SLEffectSendItf bqPlayerEffectSend;
    static SLMuteSoloItf bqPlayerMuteSolo;
    static SLVolumeItf bqPlayerVolume;
    static void *buffer;
    static size_t bufferSize;


private:
    int initFFmpeg(int *rate, int *channel, const char *url);

    void initOpenSLES();

    void initBufferQueue(int rate, int channel, int bitsPerSample);

    // this callback handler is called every time a buffer finishes playing
    static void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);

    int getPCM(void **pcm, size_t *pcmSize);

    int releaseFFmpeg();

public:

    int playAudio(const char *inputUrl);

    void stopAudio();


};


#endif //MYYFFMPEG_FFMPEGPLAYAUDIO_H
