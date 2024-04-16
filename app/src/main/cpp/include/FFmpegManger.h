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
#include "ffmpegpch.h"

class FFmpegManger {
private:
    const char *inputUrl;


public:
    const char *getInputUrl() const;
    void setInputUrl(const char *inputUrl);
    int initFFmpeg();
    int mp4ConversionAvi(const char *inputUrl,const char *outputUrl);
    int mp4WaterMark(const char *inputUrl,const char *pngUrl,const char *outputUrl);

};


#endif //MYYFFMPEG_FFMPEGMANGER_H
