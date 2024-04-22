//
// Created by MMM on 2024/4/15.
//

#ifndef MYYFFMPEG_FFMPEGMANGER_H
#define MYYFFMPEG_FFMPEGMANGER_H
#define LOG_TAG "wy"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#include <android/log.h>
#include <jni.h>
#include <android/native_window_jni.h>
#include "ffmpegpch.h"
#include "FFmpegVideoTrans.h"
#include "FFmpegWaterMark.h"
#include "FFmpegPlayAudio.h"
#include "FFmpegWaterMarkTest.h"
#include "FFmpegVideoPlay.h"


class FFmpegManger {
private:
    FFmpegVideoTrans *fFmpegVideoTrans = nullptr;
    FFmpegWaterMarkTest *waterMark = nullptr;
    FFmpegPlayAudio *fmpegPlayAudio = nullptr;
    FFmpegVideoPlay *videoPlay = nullptr;
    AVCodecContext *mAvCodecContext = nullptr;

    const char *inputUrl;


public:
    const char *getInputUrl() const;
    void setInputUrl(const char *inputUrl);
    int initFFmpeg();
    int mp4ConversionAvi(const char *inputUrl,const char *outputUrl);
    int mp4WaterMark(const char *inputUrl,const char *pngUrl,const char *outputUrl);
    int playAudio(const char *inputUrl);
    int stopAudio();
    int playVideo(JNIEnv *env, jobject surface, const char *inputUrl);

};


#endif //MYYFFMPEG_FFMPEGMANGER_H
