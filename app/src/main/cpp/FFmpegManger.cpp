//
// Created by MMM on 2024/4/15.
//

#include "include/FFmpegManger.h"


const char *FFmpegManger::getInputUrl() const {
    return inputUrl;
}

void FFmpegManger::setInputUrl(const char *inputUrl) {
    FFmpegManger::inputUrl = inputUrl;
}



int FFmpegManger::mp4ConversionAvi(const char *in_filename, const char *out_filename) {
    if (fFmpegVideoTrans == nullptr) {
        fFmpegVideoTrans = new FFmpegVideoTrans();
    }
    int i = fFmpegVideoTrans->mp4ConversionAvi(in_filename, out_filename);
    return i;
}

int FFmpegManger::mp4WaterMark(const char *inputUrl, const char *pngUrl, const char *outputUrl) {
//    FFmpegWaterMark *waterMark = new FFmpegWaterMark();
//    int ret = waterMark->mp4WaterMark(inputUrl, pngUrl, outputUrl);
    if (waterMark == nullptr) {
        waterMark = new FFmpegWaterMarkTest();
    }
    waterMark->mp4WaterMark(inputUrl, pngUrl, outputUrl);

//    FFmpegWaterMarkTest *waterMark = new FFmpegWaterMarkTest();
//    waterMark->mp4WaterMark_leixiaohua(inputUrl, pngUrl, outputUrl);
    return 0;
}

int FFmpegManger::playAudio(const char *inputUrl) {
    if (fmpegPlayAudio == nullptr) {
        fmpegPlayAudio = new FFmpegPlayAudio();
    }
    fmpegPlayAudio->playAudio(inputUrl);
    return 0;
}

int FFmpegManger::stopAudio() {
    if (fmpegPlayAudio == nullptr) {
        return -1;
    }
    fmpegPlayAudio->stopAudio();
    return 0;
}

int FFmpegManger::playVideo() {
    if (videoPlay != nullptr) {
        videoPlay->playVideo();
    }
    return 0;
}

int FFmpegManger::stopVideo() {
    if (videoPlay == nullptr) {
        return -1;
    }
    videoPlay->stopVideo();
    return 0;
}

int FFmpegManger::pauseVideo() {
    if (videoPlay == nullptr) {
        return -1;
    }
    videoPlay->pauseVideo();
    return 0;
}

void FFmpegManger::SetMessageCallback(void *context, MessageCallback callback) {
    mMsgContext = context;
    mMsgCallback = callback;
}

int FFmpegManger::initFFmpeg(JNIEnv *env,jobject thiz, const char *inputUrl, jobject surface) {
    if (videoPlay == nullptr) {
        videoPlay = new FFmpegVideoPlay();
        videoPlay->init(env, thiz, inputUrl,surface);
    }
    return 0;
}

void FFmpegManger::unInit() {
    if (videoPlay != nullptr) {
        videoPlay->unInit();
    }
}
