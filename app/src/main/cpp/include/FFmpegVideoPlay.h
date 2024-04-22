//
// Created by MMM on 2024/4/19.
//

#ifndef MYYFFMPEG_FFMPEGVIDEOPLAY_H
#define MYYFFMPEG_FFMPEGVIDEOPLAY_H
#include "ffmpegpch.h"
#include "jni.h"
#include "android/native_window.h"


class FFmpegVideoPlay {
private:
    int ret = -1;
    int mVideoIndex = -1;
    int mWidth;
    int mHeight;
    uint8_t *mOutbuffer;
    JNIEnv *mEnv = nullptr;
    SwsContext *mSwsContext = nullptr;
    jobject androidSurface = NULL;
    const char *mInputUrl = nullptr;
    AVFormatContext *mAvFormatContext = nullptr;
    AVCodecContext *mAvCodecContext = nullptr;
    ANativeWindow_Buffer windowBuffer;
    ANativeWindow *mNativeWindow = nullptr;
    AVFrame *mAvFrame = nullptr;
    AVPacket *mAvPacket = nullptr;
    AVFrame *mRgbFrame = nullptr;

public:
    void initFFmeg(JNIEnv *env, jobject surface, const char *inputUrl);

    int initFormatContext();

    int initFFmpegCodec();

    int playVideo();

    int initANativeWindow();


    ~FFmpegVideoPlay();
};


#endif //MYYFFMPEG_FFMPEGVIDEOPLAY_H
