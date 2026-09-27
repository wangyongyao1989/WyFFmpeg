//
// Created by MMM on 2024/4/19.
//

#include <android/native_window_jni.h>
#include "include/FFmpegVideoPlay.h"

using namespace std;

int FFmpegVideoPlay::playVideo() {
    if (decodecThread == nullptr) {
        LOGD("playVideo: recreate decode thread %p", decodecThread);
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            pauseFlag = false;
            stopFlag = false;
        }
        decodecThread = new thread(DoAVdecoding, this);
    } else {
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            pauseFlag = false;
            stopFlag = false;
        }
        m_Cond.notify_all();
    }
    mPlayerState = PLAYER_STATE_PLAYING;
    if (mMsgContext && mPlayStatusCallback)
        mPlayStatusCallback(mMsgContext, "playVideo");
    return 0;
}

void FFmpegVideoPlay::stopVideo() {
    // 停止并回收解码线程，再回到片头，保证下次 playVideo 可重播
    if (decodecThread != nullptr) {
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            pauseFlag = false;
            stopFlag = true;
        }
        m_Cond.notify_all();
        decodecThread->join();
        delete decodecThread;
        decodecThread = nullptr;
    }
    m_SeekPosition = 0;
    if (mAvFormatContext != nullptr && mAvCodecContext != nullptr) {
        doSeekTo(0);
    }
    mPlayerState = PLAYER_STATE_STOP;
    if (mMsgContext && mPlayStatusCallback)
        mPlayStatusCallback(mMsgContext, "stopVideo");
}

void FFmpegVideoPlay::pauseVideo() {
    if (decodecThread != nullptr) {
        std::lock_guard<std::mutex> lock(m_Mutex);
        pauseFlag = true;
    }
    mPlayerState = PLAYER_STATE_PAUSE;
    if (mMsgContext && mPlayStatusCallback)
        mPlayStatusCallback(mMsgContext, "pauseVideo");
}

int FFmpegVideoPlay::seekToPosition(float position) {
    if (position < 0) position = 0;
    if (position > 100) position = 100;
    m_SeekPosition = position;
    if (decodecThread != nullptr) {
        std::lock_guard<std::mutex> lock(m_Mutex);
        pauseFlag = true;
        m_Cond.notify_all();// 在锁内 notify，消除“判断谓词与进入 wait 之间”的丢唤醒窗口
    }
    mPlayerState = PLAYER_STATE_PAUSE;
    if (mMsgContext && mPlayStatusCallback)
        mPlayStatusCallback(mMsgContext, "seekToPosition pauseVideo");
    return 0;
}

int64_t FFmpegVideoPlay::getDurationUs() {
    if (mAvFormatContext != nullptr && mAvFormatContext->duration > 0) {
        return mAvFormatContext->duration;
    }
    if (mAvFormatContext != nullptr && mVideoIndex >= 0
        && mVideoIndex < mAvFormatContext->nb_streams) {
        AVStream *st = mAvFormatContext->streams[mVideoIndex];
        if (st->duration > 0 && st->time_base.den != 0) {
            return av_rescale_q(st->duration, st->time_base, AVRational{1, 1000000});
        }
    }
    return 0;
}

void FFmpegVideoPlay::doSeekTo(int64_t timestampUs) {
    if (mAvFormatContext == nullptr) {
        return;
    }
    if (avformat_seek_file(mAvFormatContext, -1, INT64_MIN, timestampUs, INT64_MAX, 0) < 0) {
        LOGD("doSeekTo(%lld) failed", (long long) timestampUs);
        return;
    }
    if (-1 != mVideoIndex && mAvCodecContext != nullptr) {
        avcodec_flush_buffers(mAvCodecContext);
    }
}

void FFmpegVideoPlay::doSeek() {
    float position = m_SeekPosition;
    int64_t durationUs = getDurationUs();
    int64_t seek_target;
    if (durationUs > 0) {
        // 进度条是 0-100 的百分比，按视频时长换算成微秒
        seek_target = static_cast<int64_t>(durationUs * position / 100.0);
        int64_t maxTarget = durationUs - 1000000;  // 预留 1s，避免 seek 到结尾立刻 EOF
        if (maxTarget > 0 && seek_target > maxTarget) {
            seek_target = maxTarget;
        }
    } else {
        // 时长未知（如流媒体）时退回按秒处理
        seek_target = static_cast<int64_t>(position * 1000000);
    }
    if (mAvFormatContext != nullptr
        && avformat_seek_file(mAvFormatContext, -1, INT64_MIN, seek_target, INT64_MAX, 0) < 0) {
        LOGD("SeekPosition 失败");
    } else if (-1 != mVideoIndex) {
        avcodec_flush_buffers(mAvCodecContext);
    }
    m_SeekPosition = 0;  // 失败也清零，避免死循环重试
    LOGD("seek to %.1f%% done", position);
}

int FFmpegVideoPlay::initFormatContext() {
    mAvFormatContext = avformat_alloc_context();
    //1.打开文件
    ret = avformat_open_input(&mAvFormatContext, mInputUrl, nullptr, nullptr);
    if (ret < 0) {
        LOGD("Couldn't open input stream. ret:%d", ret);
    }
    LOGD("open inputFile:%s", mInputUrl);
    //2.获取音视频流
    ret = avformat_find_stream_info(mAvFormatContext, nullptr);
    if (ret < 0) {
        LOGD("Couldn't find stream info. ret:%d", ret);
        return ret;
    }
    av_dump_format(mAvFormatContext,0,0,0);
    return ret;
}

int FFmpegVideoPlay::initFFmpegCodec() {
    //3.寻找流的索引
    for (int i = 0; i < mAvFormatContext->nb_streams; ++i) {
        AVMediaType mediaType = mAvFormatContext->streams[i]->codecpar->codec_type;
        LOGD("find % d stream mediaType. i:%d", i, mediaType);
        if (mediaType == AVMEDIA_TYPE_VIDEO) {
            mVideoIndex = i;
            break;
        }
    }

    if (mVideoIndex < 0) {
        LOGD("Couldn't find a video stream.\n");
        return mVideoIndex;
    }

    //4.找到解码器上下文
    AVCodecParameters *pParameters = mAvFormatContext->streams[mVideoIndex]->codecpar;
    AVCodecID avCodecId = pParameters->codec_id;
    AVCodec *findDecoder = avcodec_find_decoder(avCodecId);
    mAvCodecContext = avcodec_alloc_context3(findDecoder);
    avcodec_parameters_to_context(mAvCodecContext, pParameters);
    AVDictionary *pAVDictionary = nullptr;
    av_dict_set(&pAVDictionary, "buffer_size", "1024000", 0);
    av_dict_set(&pAVDictionary, "stimeout", "20000000", 0);
    av_dict_set(&pAVDictionary, "max_delay", "30000000", 0);
    av_dict_set(&pAVDictionary, "rtsp_transport", "tcp", 0);
    //5.打开解码器
    ret = avcodec_open2(mAvCodecContext, findDecoder, &pAVDictionary);
    if (ret < 0) {
        LOGD("Couldn't open codec.\n");
        return ret;
    }
    LOGD("打开解码成功\n");
    return ret;
}

void FFmpegVideoPlay::initFFmeg() {
    initFormatContext();
    initFFmpegCodec();
    initANativeWindow();
}

int FFmpegVideoPlay::initANativeWindow() {
    //6.初始化ANativeWindow
    mNativeWindow = ANativeWindow_fromSurface(mEnv, androidSurface);
    if (nullptr == mNativeWindow) {
        LOGD("Couldn't get native window from surface.\n");
        return -1;
    }
    LOGD("获取到 native window from surface\n");
    mAvFrame = av_frame_alloc();
    mAvPacket = av_packet_alloc();
    mRgbFrame = av_frame_alloc();

    mWidth = mAvCodecContext->width;
    mHeight = mAvCodecContext->height;
    LOGD("width： %d，==height:%d", mWidth, mHeight);

    int bufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGBA, mWidth, mHeight, 1);
    LOGD("计算解码后的rgb %d\n", bufferSize);
    mOutbuffer = (uint8_t *) av_malloc(bufferSize * sizeof(uint8_t));

    //   转换器
    mSwsContext = sws_getContext(mWidth, mHeight, mAvCodecContext->pix_fmt,
                                 mWidth, mHeight, AV_PIX_FMT_RGBA,
                                 SWS_BICUBIC, nullptr, nullptr,
                                 nullptr);
    int32_t i = ANativeWindow_setBuffersGeometry(mNativeWindow, mWidth, mHeight,
                                                 WINDOW_FORMAT_RGBA_8888);
    if (0 > i) {
        LOGD("Couldn't set buffers geometry.\n");
        ANativeWindow_release(mNativeWindow);
        return -1;
    }
    LOGD("ANativeWindow_setBuffersGeometry成功\n");
    return 0;
}

FFmpegVideoPlay::~FFmpegVideoPlay() {
    // 先停线程再释放资源，避免解码线程正在使用 context 时被释放（use-after-free）
    joinDecodecThread();
    releaseResources();
    mEnv = nullptr;
    androidSurface = nullptr;
    mInputUrl = nullptr;
    mInputUrlStorage.clear();
}

void FFmpegVideoPlay::DoAVdecoding(FFmpegVideoPlay *fmpegVideoPlay) {
    fmpegVideoPlay->loopDecodec();
}


int FFmpegVideoPlay::loopDecodec() {
    av_image_fill_arrays(mRgbFrame->data, mRgbFrame->linesize,
                         mOutbuffer, AV_PIX_FMT_RGBA,
                         mWidth, mHeight, 1);

    while (true) {
        if (stopFlag) {
            break;
        }
        if (m_SeekPosition > 0) {
            // 暂停状态下也要先消费 seek，恢复播放时才能出现在目标位置
            doSeek();
        }
        if (pauseFlag) {
            unique_lock<mutex> uniqueLock(m_Mutex);
            while (pauseFlag && !stopFlag) {
                // 带 100ms 超时的等待：即使出现 notify 丢失/竞态，也会被下一次超时唤醒，
                // 从而消费 m_SeekPosition，不会永久错过后续的 seek 请求
                m_Cond.wait_for(uniqueLock, std::chrono::milliseconds(100));
                if (m_SeekPosition > 0) break;// 有新 seek 时立即退出等待去处理
            }
            uniqueLock.unlock();
        }
        if (stopFlag) {
            break;
        }

        int result = av_read_frame(mAvFormatContext, mAvPacket);
        if (result == AVERROR(EAGAIN)) {
            continue;
        }
        if (result < 0) {
            // EOF 或读取错误：回到片头并暂停，对外回调播放完成，支持再次播放
            LOGD("read frame finished：%d, playback complete", result);
            doSeekTo(0);
            {
                std::lock_guard<std::mutex> lock(m_Mutex);
                pauseFlag = true;
            }
            mPlayerState = PLAYER_STATE_STOP;
            if (mMsgContext && mPlayStatusCallback)
                mPlayStatusCallback(mMsgContext, "playVideo complete");
            continue;
        }
        if (mAvPacket->stream_index == mVideoIndex) {
            int ret = codecAvFrame();
            if (ret == AVERROR(EAGAIN)) {
                av_packet_unref(mAvPacket);
                continue;
            } else if (ret < 0) {
                // 单帧解码失败跳过该帧，而不是终止整个播放线程
                LOGD("codecAvFrame fail, skip frame");
                av_packet_unref(mAvPacket);
                continue;
            }
            sendFrameDataToANativeWindow();
            av_frame_unref(mAvFrame);
        }
        av_packet_unref(mAvPacket);
    }
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        pauseFlag = false;
        stopFlag = false;
    }
    return 0;
}

int FFmpegVideoPlay::codecAvFrame() {
    ret = avcodec_send_packet(mAvCodecContext, mAvPacket);
    if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
        LOGD("codecAvFrame 解码出错");
        return -1;
    }
    ret = avcodec_receive_frame(mAvCodecContext, mAvFrame);
    return ret;
}

int FFmpegVideoPlay::sendFrameDataToANativeWindow() {
    // 未压缩的数据
    sws_scale(mSwsContext, mAvFrame->data,
              mAvFrame->linesize, 0,
              mAvCodecContext->height, mRgbFrame->data,
              mRgbFrame->linesize);
    auto lock = ANativeWindow_lock(mNativeWindow, &windowBuffer, nullptr);
    if (lock < 0) {
        LOGD("cannot lock window");
    } else {
        //将图像绘制到界面上，注意这里pFrameRGBA一行的像素和windowBuffer一行的像素长度可能不一致
        //需要转换好，否则可能花屏
        auto *dst = (uint8_t *) windowBuffer.bits;
        for (int h = 0; h < mHeight; h++) {
            memcpy(dst + h * windowBuffer.stride * 4,
                   mOutbuffer + h * mRgbFrame->linesize[0],
                   mRgbFrame->linesize[0]);
        }
        // 逐 I 帧跨 JNI 回调会刷屏并有 local ref 泄漏风险，帧类型变化不再回调；
        // 播放状态（play/pause/stop/complete）已在各控制方法中回调。
        switch (mAvFrame->pict_type) {
            default:
                break;
        }
    }
    av_usleep(1000 * 33);
    ANativeWindow_unlockAndPost(mNativeWindow);
    return lock;
}

void FFmpegVideoPlay::init(JNIEnv *env, jobject thiz, const char *inputUrl, jobject surface) {
    if (mAvFormatContext != nullptr) {
        // 防止重复 init（例如未走 surfaceDestroyed 就直接再进入播放页）导致旧资源泄漏
        joinDecodecThread();
        releaseResources();
    }
    mEnv = env;
    jniClazz = thiz;
    androidSurface = surface;
    mInputUrlStorage = inputUrl == nullptr ? "" : inputUrl;
    mInputUrl = mInputUrlStorage.c_str();
    initFFmeg();
}

void FFmpegVideoPlay::unInit() {
    // 先停止并回收解码线程，再释放资源；顺序颠倒会造成野指针/双重释放
    joinDecodecThread();
    releaseResources();
}

void FFmpegVideoPlay::joinDecodecThread() {
    if (decodecThread != nullptr) {
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            pauseFlag = false;
            stopFlag = true;
        }
        m_Cond.notify_all();
        decodecThread->join();
        delete decodecThread;
        decodecThread = nullptr;
    }
}

void FFmpegVideoPlay::releaseResources() {
    sws_freeContext(mSwsContext);
    mSwsContext = nullptr;
    avcodec_free_context(&mAvCodecContext);
    if (mAvFormatContext != nullptr) {
        avformat_close_input(&mAvFormatContext);
    }
    av_frame_free(&mRgbFrame);
    av_frame_free(&mAvFrame);
    av_packet_free(&mAvPacket);
    if (mOutbuffer != nullptr) {
        av_free(mOutbuffer);
        mOutbuffer = nullptr;
    }
    if (mNativeWindow != nullptr) {
        ANativeWindow_release(mNativeWindow);
        mNativeWindow = nullptr;
    }
    mPlayerState = PLAYER_STATE_UNKNOWN;
}

void FFmpegVideoPlay::initCallback(JNIEnv *env, jobject thiz) {
    mEnv = env;
    jniClazz = thiz;
}

void FFmpegVideoPlay::SetMessageCallback(void *context, MessageCallback callback) {
    mMsgContext = context;
    mMsgCallback = callback;
}

void FFmpegVideoPlay::SetPlayStatusCallback(void *context, PlaystatusCallback callback) {
    mMsgContext = context;
    mPlayStatusCallback = callback;
}


