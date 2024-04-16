//
// Created by MMM on 2024/4/15.
//

#include "include/FFmpegManger.h"
#include "FFmpegVideoTrans.h"
#include "FFmpegWaterMark.h"

const char *FFmpegManger::getInputUrl() const {
    return inputUrl;
}

void FFmpegManger::setInputUrl(const char *inputUrl) {
    FFmpegManger::inputUrl = inputUrl;
}

int FFmpegManger::initFFmpeg() {


}

int FFmpegManger::mp4ConversionAvi(const char *in_filename, const char *out_filename) {
    FFmpegVideoTrans *fFmpegVideoTrans = new FFmpegVideoTrans();
    int i = fFmpegVideoTrans->mp4ConversionAvi(in_filename, out_filename);
    return i;
}

int FFmpegManger::mp4WaterMark(const char *inputUrl, const char *pngUrl, const char *outputUrl) {
    FFmpegWaterMark *waterMark = new FFmpegWaterMark();
    int ret = waterMark->mp4WaterMark(inputUrl, pngUrl, outputUrl);
    return 0;
}
