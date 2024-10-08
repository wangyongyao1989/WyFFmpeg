//
// Created by MMM on 2024/4/19.
//

#include <android/native_window_jni.h>
#include "include/FFmpegVideoPlay.h"

using namespace std;

int FFmpegVideoPlay::playVideo() {
    if (decodecThread == nullptr) {
        LOGD("playVideo=========%p", decodecThread);
        decodecThread = new thread(DoAVdecoding, this);
//        decodecThread->detach();
    }
    if (pauseFlag) {
        pauseFlag = false;
        m_Cond.notify_all();
    }
    if (stopFlag) {
        stopFlag = false;
        m_Cond.notify_all();
    }
    mPlayerState = PLAYER_STATE_PLAYING;
    if (mMsgContext && mPlayStatusCallback)
        mPlayStatusCallback(mMsgContext, "playVideo");
    return 0;
}

void FFmpegVideoPlay::stopVideo() {
    if (decodecThread != nullptr) {
        pauseFlag = false;
        stopFlag = true;
        m_Cond.notify_all();
    }
    mPlayerState = PLAYER_STATE_STOP;
    if (mMsgContext && mPlayStatusCallback)
        mPlayStatusCallback(mMsgContext, "stopVideo");
}

void FFmpegVideoPlay::pauseVideo() {
    if (decodecThread != nullptr) {
        pauseFlag = true;
    }
    mPlayerState = PLAYER_STATE_PAUSE;
    if (mMsgContext && mPlayStatusCallback)
        mPlayStatusCallback(mMsgContext, "pauseVideo");
}

int FFmpegVideoPlay::seekToPosition(float position) {
    if (decodecThread != nullptr) {
        pauseFlag = true;
    }
    mPlayerState = PLAYER_STATE_PAUSE;
    m_SeekPosition = position;
    m_Cond.notify_all();
    if (mMsgContext && mPlayStatusCallback)
        mPlayStatusCallback(mMsgContext, "seekToPosition pauseVideo");
    return 0;
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
    mEnv = nullptr;
    androidSurface = nullptr;
    mInputUrl = nullptr;
    sws_freeContext(mSwsContext);
    avcodec_free_context(&mAvCodecContext);
    avformat_free_context(mAvFormatContext);
    av_frame_free(&mRgbFrame);
    av_frame_free(&mAvFrame);
    av_packet_free(&mAvPacket);
    if (decodecThread != nullptr) {
        pauseFlag = false;
        stopFlag = true;
        m_Cond.notify_all();
        decodecThread->join();
        delete decodecThread;
        decodecThread = nullptr;
    }
}

void FFmpegVideoPlay::DoAVdecoding(FFmpegVideoPlay *fmpegVideoPlay) {
    fmpegVideoPlay->loopDecodec();
}


int FFmpegVideoPlay::loopDecodec() {
    av_image_fill_arrays(mRgbFrame->data, mRgbFrame->linesize,
                         mOutbuffer, AV_PIX_FMT_RGBA,
                         mWidth, mHeight, 1);

    while (true) {
        if (pauseFlag) {
            unique_lock<mutex> uniqueLock(m_Mutex);
            while (pauseFlag) {
                m_Cond.wait(uniqueLock); // Unlock _mutex and wait to be notified
            }
            uniqueLock.unlock();
        }
        if (stopFlag) {
            break;
        }
        if (m_SeekPosition > 0) {
            //seek to frame
            int64_t seek_target = static_cast<int64_t>(m_SeekPosition * 1000000);//微秒
            int64_t seek_min = INT64_MIN;
            int64_t seek_max = INT64_MAX;
            int seek_ret = avformat_seek_file(mAvFormatContext, -1,
                                              seek_min, seek_target,
                                              seek_max,
                                              0);
            if (seek_ret < 0) {
                LOGD("SeekPosition 失败");
            } else {
                if (-1 != mVideoIndex) {
                    avcodec_flush_buffers(mAvCodecContext);
                }
                m_SeekPosition = 0;
            }
        }

        int result = av_read_frame(mAvFormatContext, mAvPacket);
        if (result < 0) {
            LOGD("av_read_frame fail：%d",result);
            break;
        }
        if (mAvPacket->stream_index == mVideoIndex) {
            int ret = codecAvFrame();
            if (ret == AVERROR(EAGAIN)) {
                continue;
            } else if (ret < 0) {
                LOGD("codecAvFrame fail");
                break;
            }
            sendFrameDataToANativeWindow();
        }
    }
    pauseFlag = false;
    stopFlag = false;
    avformat_free_context(mAvFormatContext);
    return ret;
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
        switch (mAvFrame->pict_type) {
            case AV_PICTURE_TYPE_I:
                if (mMsgContext && mPlayStatusCallback)
                    mPlayStatusCallback(mMsgContext, "读取到I帧");
                break;
            case AV_PICTURE_TYPE_P:
//                if (mMsgContext && mPlayStatusCallback)
//                    mPlayStatusCallback(mMsgContext, "读取到P帧");
                break;
            case AV_PICTURE_TYPE_B:
//                if (mMsgContext && mPlayStatusCallback)
//                    mPlayStatusCallback(mMsgContext, "读取到B帧");
                break;
            default:;
                break;
        }
    }
    av_usleep(1000 * 33);
    ANativeWindow_unlockAndPost(mNativeWindow);
    return lock;
}

void FFmpegVideoPlay::init(JNIEnv *env, jobject thiz, const char *inputUrl, jobject surface) {
    mEnv = env;
    jniClazz = thiz;
    androidSurface = surface;
    mInputUrl = inputUrl;
    initFFmeg();
}

void FFmpegVideoPlay::unInit() {
    sws_freeContext(mSwsContext);
    avcodec_free_context(&mAvCodecContext);
    avformat_free_context(mAvFormatContext);
    av_frame_free(&mRgbFrame);
    av_frame_free(&mAvFrame);
    av_packet_free(&mAvPacket);
    if (decodecThread != nullptr) {
        pauseFlag = false;
        stopFlag = true;
        m_Cond.notify_all();
        decodecThread->detach();
        delete decodecThread;
        decodecThread = nullptr;
    }
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


