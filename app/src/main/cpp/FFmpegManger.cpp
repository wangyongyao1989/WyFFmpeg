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

int FFmpegManger::initFFmpeg() {

}
