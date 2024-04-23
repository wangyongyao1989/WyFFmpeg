//
// Created by MMM on 2024/4/19.
//

#ifndef MYYFFMPEG_FFMPEGVIDEOPLAY_H
#define MYYFFMPEG_FFMPEGVIDEOPLAY_H

#include "ffmpegpch.h"
#include "jni.h"
#include "android/native_window.h"
#include <thread>

enum PlayerState {
    PLAYER_STATE_UNKNOWN,
    PLAYER_STATE_PLAYING,
    PLAYER_STATE_PAUSE,
    PLAYER_STATE_STOP
};

class FFmpegVideoPlay {
private:
    int ret = -1;
    int mVideoIndex = -1;
    int mWidth;
    int mHeight;
    uint8_t *mOutbuffer;
    JNIEnv *mEnv = nullptr;
    SwsContext *mSwsContext = nullptr;
    jobject androidSurface = NULL;
    const char *mInputUrl = nullptr;
    AVFormatContext *mAvFormatContext = nullptr;
    AVCodecContext *mAvCodecContext = nullptr;
    ANativeWindow_Buffer windowBuffer;
    ANativeWindow *mNativeWindow = nullptr;
    AVFrame *mAvFrame = nullptr;
    AVPacket *mAvPacket = nullptr;
    AVFrame *mRgbFrame = nullptr;

    std::thread *decodecThread = nullptr;
    PlayerState mPlayerState = PLAYER_STATE_UNKNOWN;
    std::mutex               m_Mutex;
    std::condition_variable  m_Cond;



public:
    void initFFmeg(JNIEnv *env, jobject surface, const char *inputUrl);

    int initFormatContext();

    int initFFmpegCodec();

    int playVideo();

    void pauseVideo();

    void stopVideo();

    int codecAvFrame();

    int initANativeWindow();

    int sendFrameDataToANativeWindow();

    int loopDecodec();

    static void DoAVdecoding(FFmpegVideoPlay *fmpegVideoPlay);

    ~FFmpegVideoPlay();
};


#endif //MYYFFMPEG_FFMPEGVIDEOPLAY_H
