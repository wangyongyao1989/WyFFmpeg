//
// Created by MMM on 2024/4/15.
//

#ifndef MYYFFMPEG_FFMPEGMANGER_H
#define MYYFFMPEG_FFMPEGMANGER_H
#include <android/log.h>

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libswresample/swresample.h"
}

class FFmpegManger {
private:
    const char *inputUrl;


public:
    const char *getInputUrl() const;
    void setInputUrl(const char *inputUrl);
    int initFFmpeg();
    int mp4ConversionAvi(const char *inputUrl,const char *outputUrl);

};


#endif //MYYFFMPEG_FFMPEGMANGER_H
