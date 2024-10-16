//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2024/10/15.
// 参考于：https://fossies.org/linux/opencv/modules/videoio/src/cap_android_mediandk.cpp
//

#ifndef MYYFFMPEG_ANDROIDMEDIANDKCAPTURE_H
#define MYYFFMPEG_ANDROIDMEDIANDKCAPTURE_H


#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <android/log.h>
#include <android/native_window.h>
#include "media/NdkMediaCodec.h"
#include "media/NdkMediaMuxer.h"
#include "media/NdkMediaExtractor.h"
#include "media/NdkMediaFormat.h"
#include "LogUtils.h"


#define INPUT_TIMEOUT_MS 2000

#define COLOR_FormatSurface 0x7f000789
//See https://developer.android.com/reference/android/media/MediaCodecInfo.CodecCapabilities for codes
#define COLOR_FormatYUV420Planar 19
#define COLOR_FormatYUV420SemiPlanar 21
#define TAG "NativeCodec"

static inline void deleter_AMediaExtractor(AMediaExtractor *extractor) {
    AMediaExtractor_delete(extractor);
}

static inline void deleter_AMediaCodec(AMediaCodec *codec) {
    AMediaCodec_stop(codec);
    AMediaCodec_delete(codec);
}

static inline void deleter_AMediaFormat(AMediaFormat *format) {
    AMediaFormat_delete(format);
}

//using namespace cv;

class AndroidMediaNdkCapture {

public:

    AndroidMediaNdkCapture();

    ~AndroidMediaNdkCapture();


    std::shared_ptr<AMediaExtractor> mediaExtractor;
    std::shared_ptr<AMediaCodec> mediaCodec;
    bool sawInputEOS;
    bool sawOutputEOS;
    int32_t frameStride;
    int32_t frameWidth;
    int32_t frameHeight;
    int32_t colorFormat;
    int32_t videoWidth;
    int32_t videoHeight;
    float videoFrameRate;
    int32_t videoFrameCount;
    int32_t videoRotation;
    int32_t videoRotationCode;
    bool videoOrientationAuto;
    std::vector<uint8_t> buffer;
//    Mat frame;

private:

    bool decodeFrame();

    bool initCapture(const char *filename);

    void cleanUp();

};


#endif //MYYFFMPEG_ANDROIDMEDIANDKCAPTURE_H
