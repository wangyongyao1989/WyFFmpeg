//
// Created by MMM on 2024/4/19.
//

#include <android/native_window_jni.h>
#include "include/FFmpegVideoPlay.h"

using namespace std;

int FFmpegVideoPlay::playVideo() {
    mPlayerState = PLAYER_STATE_PLAYING;
    if (decodecThread != nullptr) {
        decodecThread->detach();
    } else {
        decodecThread = new thread(DoAVdecoding, this);
        decodecThread->detach();
    }
    return 0;
}

void FFmpegVideoPlay::stopVideo() {

}

int FFmpegVideoPlay::initFormatContext() {
    mAvFormatContext = avformat_alloc_context();
    //1.打开文件
    ret = avformat_open_input(&mAvFormatContext, mInputUrl, nullptr, nullptr);
    if (ret < 0) {
        LOGD("Couldn't open input stream. ret:%d", ret);
    }
    LOGD("open inputFile:%s", mInputUrl);
    //2.查找文件流信息
    ret = avformat_find_stream_info(mAvFormatContext, nullptr);
    if (ret < 0) {
        LOGD("Couldn't find stream info. ret:%d", ret);
        return ret;
    }
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

    //5.打开解码器
    ret = avcodec_open2(mAvCodecContext, findDecoder, nullptr);
    if (ret < 0) {
        LOGD("Couldn't open codec.\n");
        return ret;
    }
    LOGD("打开解码成功\n");
    return ret;
}

void FFmpegVideoPlay::initFFmeg(JNIEnv *env, jobject surface, const char *inputUrl) {
    mEnv = env;
    androidSurface = surface;
    mInputUrl = inputUrl;
    initFormatContext();
    initFFmpegCodec();
    initANativeWindow();
    if (decodecThread == nullptr) {
        decodecThread = new thread(DoAVdecoding, this);
    }
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
}

int FFmpegVideoPlay::decodecFrameAndShowWindow() {
    LOGD("decodecFrameAndShowWindow,");
    av_image_fill_arrays(mRgbFrame->data, mRgbFrame->linesize,
                         mOutbuffer, AV_PIX_FMT_RGBA,
                         mWidth, mHeight, 1);
    while (av_read_frame(mAvFormatContext, mAvPacket) >= 0) {
//        读出来的数据是什么数据 视频   音频数据不管
        if (mAvPacket->stream_index == mVideoIndex) {
            ret = avcodec_send_packet(mAvCodecContext, mAvPacket);
            if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
                LOGD("解码出错");
                return -1;
            }

            ret = avcodec_receive_frame(mAvCodecContext, mAvFrame);
            if (ret == AVERROR(EAGAIN)) {
                continue;
            } else if (ret < 0) {
                break;
            }
//            未压缩的数据
            sws_scale(mSwsContext, mAvFrame->data,
                      mAvFrame->linesize, 0,
                      mAvCodecContext->height, mRgbFrame->data,
                      mRgbFrame->linesize);
            if (ANativeWindow_lock(mNativeWindow, &windowBuffer, nullptr) < 0) {
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
                        LOGD("I");
                        break;
                    case AV_PICTURE_TYPE_P:
                        LOGD("P");
                        break;
                    case AV_PICTURE_TYPE_B:
                        LOGD("B");
                        break;
                    default:;
                        break;
                }
            }
            av_usleep(1000 * 33);
            ANativeWindow_unlockAndPost(mNativeWindow);
        }

    }

    avformat_free_context(mAvFormatContext);
    return ret;
}

void FFmpegVideoPlay::DoAVdecoding(FFmpegVideoPlay *fmpegVideoPlay) {
    fmpegVideoPlay->decodecFrameAndShowWindow();
}

