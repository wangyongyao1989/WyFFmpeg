//
// Created by MMM on 2024/4/19.
//

#ifndef MYYFFMPEG_FFMPEGVIDEOPLAY_H
#define MYYFFMPEG_FFMPEGVIDEOPLAY_H

#include "ffmpegpch.h"
#include "jni.h"
#include "android/native_window.h"
#include <thread>
#include <string>
#include "GLES3/gl3.h"
#include "GLES3/gl3ext.h"
#include "GLES3/gl3platform.h"

enum PlayerState {
    PLAYER_STATE_UNKNOWN,
    PLAYER_STATE_PLAYING,
    PLAYER_STATE_PAUSE,
    PLAYER_STATE_STOP
};

typedef void (*MessageCallback)(void *, int, float);

typedef void (*PlaystatusCallback)(void *, const char *);

class FFmpegVideoPlay {
private:
    int ret = -1;
    int mVideoIndex = -1;
    int mWidth;
    int mHeight;
    uint8_t *mOutbuffer;
    JNIEnv *mEnv = nullptr;
    jobject jniClazz = NULL;
    jobject androidSurface = NULL;
    SwsContext *mSwsContext = nullptr;
    const char *mInputUrl = nullptr;
    std::string mInputUrlStorage;   ///<持有输入路径副本，避免 JNI 临时串被释放后悬垂
    AVFormatContext *mAvFormatContext = nullptr;
    AVCodecContext *mAvCodecContext = nullptr;
    ANativeWindow_Buffer windowBuffer;
    ANativeWindow *mNativeWindow = nullptr;
    AVFrame *mAvFrame = nullptr;
    AVPacket *mAvPacket = nullptr;
    AVFrame *mRgbFrame = nullptr;
    std::thread *decodecThread = nullptr;
    PlayerState mPlayerState = PLAYER_STATE_UNKNOWN;
    std::mutex m_Mutex;
    std::condition_variable m_Cond;
    std::atomic_bool pauseFlag;   ///<暂停标识
    std::atomic_bool stopFlag;   ///<停止标识
    volatile float m_SeekPosition = 0;   ///<目标进度，百分比 0-100


    void *mMsgContext = nullptr;
    MessageCallback mMsgCallback = nullptr;
    PlaystatusCallback mPlayStatusCallback = nullptr;

private:
    void initFFmeg();

    int initFormatContext();

    int initFFmpegCodec();

    int codecAvFrame();

    int initANativeWindow();

    int sendFrameDataToANativeWindow();

    int loopDecodec();

    void doSeek();

    void doSeekTo(int64_t timestampUs);

    int64_t getDurationUs();

    void joinDecodecThread();

    void releaseResources();

    static void do_some_work();

    static void DoAVdecoding(FFmpegVideoPlay *fmpegVideoPlay);

public:
    void init(JNIEnv *env, jobject thiz, const char *inputUrl, jobject surface);

    void unInit();

    int playVideo();

    void pauseVideo();

    void stopVideo();

    int seekToPosition(float position);

    void initCallback(JNIEnv *env, jobject thiz);

    void SetMessageCallback(void *context, MessageCallback callback);

    void SetPlayStatusCallback(void *context, PlaystatusCallback callback);

    ~FFmpegVideoPlay();
};


#endif //MYYFFMPEG_FFMPEGVIDEOPLAY_H
