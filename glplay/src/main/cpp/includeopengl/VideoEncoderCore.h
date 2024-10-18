//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2024/10/15.
// 源于google开源项目https://github.com/google/grafika 中VideoEncoderCore.java类的改造。
//

#ifndef MYYFFMPEG_VIDEOENCODERCORE_H
#define MYYFFMPEG_VIDEOENCODERCORE_H

#include <cstdint>
#include <stdio.h>
#include "LogUtils.h"
#include "thread"

#include "media/NdkMediaCrypto.h"
#include "media/NdkMediaCodec.h"
#include "media/NdkMediaError.h"
#include "media/NdkMediaFormat.h"
#include "media/NdkMediaMuxer.h"
#include "media/NdkMediaExtractor.h"
#include "WindowSurface.h"



#define COLOR_FormatSurface 0x7f000789

static const int64_t TIMEOUT = 1000L;


class VideoEncoderCore {

public:
    VideoEncoderCore(size_t width = 0, size_t height = 0, size_t bitRate = 0,
                     const char *outPutFile = nullptr);

    ~VideoEncoderCore();

    void drainEncoder(bool endOfStream);

    ANativeWindow* getInputSurface();

    void release();

private:
    size_t m_Width;
    size_t m_Height;
    size_t m_BitRate;
    const char *m_OutputFile;

    const char *MIME_TYPE = "video/avc";

    AMediaFormat *m_AMediaFormat = nullptr;

    AMediaCodec *m_AMediaCodec = nullptr;


    int mTrackIndex = -1;

    bool mMuxerStarted;

    AMediaCodecBufferInfo bufferInfo;
    ssize_t bufferIndex;
    size_t bufferSize;
    uint8_t *buffer;


    AMediaMuxer *m_AMediaMuxer;
    FILE *m_MediaMuxer_fp;
    size_t m_MediaMuxer_fd;

    ANativeWindow *m_WindowSurface = nullptr;

};


#endif //MYYFFMPEG_VIDEOENCODERCORE_H
