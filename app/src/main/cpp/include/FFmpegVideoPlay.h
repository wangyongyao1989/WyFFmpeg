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
    JNIEnv *mEnv = nullptr;
    jobject androidSurface = NULL;
    const char *mInputUrl = nullptr;
    AVFormatContext *mAvFormatContext = nullptr;
    AVCodecContext *mAvCodecContext = nullptr;


public:
    void initFFmeg(JNIEnv *env, jobject surface, const char *inputUrl);

    int initFormatContext();

    int initFFmpegCodec();

    int playVideo();


    ~FFmpegVideoPlay();
};


#endif //MYYFFMPEG_FFMPEGVIDEOPLAY_H
