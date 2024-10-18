//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2024/10/15.
//源于google开源项目https://github.com/google/grafika TextureMovieEncoder2.java类的改造。

#ifndef MYYFFMPEG_TEXTUREMOVIEENCODER2_H
#define MYYFFMPEG_TEXTUREMOVIEENCODER2_H

#include "VideoEncoderCore.h"
#include "Looper.h"

enum {
    MSG_STOP_RECORDING,
    MSG_FRAME_AVAILABLE,
};


class TextureMovieEncoder2 : public Looper {


public:
    TextureMovieEncoder2(VideoEncoderCore *videoEncoderCore);

    ~TextureMovieEncoder2();

    void stopRecording();

    bool isRecording();

    void frameAvailableSoon();

private:

    VideoEncoderCore *m_VideoEncoderCore;


    void handleFrameAvailable();

    void handleStopRecording();

    void handleMessage(LooperMessage *msg);

};


#endif //MYYFFMPEG_TEXTUREMOVIEENCODER2_H
