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


public:
    int playVideo(JNIEnv *env, jobject surface, const char *inputUrl);


};


#endif //MYYFFMPEG_FFMPEGVIDEOPLAY_H
