//
// Created by MMM on 2024/4/19.
//

#include <android/native_window_jni.h>
#include "include/FFmpegVideoPlay.h"

int FFmpegVideoPlay::playVideo() {
//    mAvFormatContext = avformat_alloc_context();
//    //1.打开文件
//    ret = avformat_open_input(&mAvFormatContext, mInputUrl, NULL, NULL);
//    if (ret < 0) {
//        LOGD("Couldn't open input stream. ret:%d", ret);
//        return ret;
//    }
//    LOGD("open inputFile:%s", mInputUrl);
//    //2.查找文件流信息
//    ret = avformat_find_stream_info(mAvFormatContext, NULL);
//    if (ret < 0) {
//        LOGD("Couldn't find stream info. ret:%d", ret);
//        return ret;
//    }

    //3.寻找流的索引
    int videoIndex = -1;
    for (int i = 0; i < mAvFormatContext->nb_streams; ++i) {
        AVMediaType mediaType = mAvFormatContext->streams[i]->codecpar->codec_type;
        LOGD("find % d stream mediaType. i:%d", i, mediaType);
        if (mediaType == AVMEDIA_TYPE_VIDEO) {
            videoIndex = i;
            break;
        }
    }

    if (videoIndex < 0) {
        LOGD("Couldn't find a video stream.\n");
        return videoIndex;
    }

    //4.找到解码器上下文
    AVCodecParameters *pParameters = mAvFormatContext->streams[videoIndex]->codecpar;
    AVCodecID avCodecId = pParameters->codec_id;
    AVCodec *findDecoder = avcodec_find_decoder(avCodecId);
    AVCodecContext *avCodecContext = avcodec_alloc_context3(findDecoder);
    avcodec_parameters_to_context(avCodecContext, pParameters);

    //5.打开解码器
    ret = avcodec_open2(avCodecContext, findDecoder, NULL);
    if (ret < 0) {
        LOGD("Couldn't open codec.\n");
        return ret;
    }
    LOGD("打开了解码成功\n");

    //6.初始化ANativeWindow
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(mEnv, androidSurface);
    if (0 == nativeWindow) {
        LOGD("Couldn't get native window from surface.\n");
        return -1;
    }
    LOGD("获取到 native window from surface\n");


    AVFrame *avFrame = av_frame_alloc();
    AVPacket *avPacket = av_packet_alloc();
    AVFrame *rgbFrame = av_frame_alloc();

    int width = avCodecContext->width;
    int height = avCodecContext->height;
    LOGD("width： %d，==height:%d", width, height);

    int bufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGBA, width, height, 1);
    LOGD("计算解码后的rgb %d\n", bufferSize);
    uint8_t *outbuffer = (uint8_t *) av_malloc(bufferSize * sizeof(uint8_t));

    av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, outbuffer, AV_PIX_FMT_RGBA, width,
                         height, 1);
    //   转换器
    SwsContext *swsContext = sws_getContext(width, height, avCodecContext->pix_fmt,
                                            width, height, AV_PIX_FMT_RGBA, SWS_BICUBIC, NULL, NULL,
                                            NULL);

    if (0 >
        ANativeWindow_setBuffersGeometry(nativeWindow, width, height, WINDOW_FORMAT_RGBA_8888)) {
        LOGD("Couldn't set buffers geometry.\n");
        ANativeWindow_release(nativeWindow);
        return -1;
    }
    LOGD("ANativeWindow_setBuffersGeometry成功\n");
    ANativeWindow_Buffer windowBuffer;

    while (av_read_frame(mAvFormatContext, avPacket) >= 0) {

//        读出来的数据是什么数据 视频   音频数据不管
        if (avPacket->stream_index == videoIndex) {
            int ret = avcodec_send_packet(avCodecContext, avPacket);
            if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
                LOGD("解码出错");
                return -1;
            }

            ret = avcodec_receive_frame(avCodecContext, avFrame);
            if (ret == AVERROR(EAGAIN)) { //我还要
                continue;
            } else if (ret < 0) {
                break;
            }
//            未压缩的数据
            sws_scale(swsContext, avFrame->data,
                      avFrame->linesize, 0,
                      avCodecContext->height, rgbFrame->data,
                      rgbFrame->linesize);
            if (ANativeWindow_lock(nativeWindow, &windowBuffer, NULL) < 0) {
                LOGD("cannot lock window");
            } else {

                //将图像绘制到界面上，注意这里pFrameRGBA一行的像素和windowBuffer一行的像素长度可能不一致
                //需要转换好，否则可能花屏
                uint8_t *dst = (uint8_t *) windowBuffer.bits;
                for (int h = 0; h < height; h++) {
                    memcpy(dst + h * windowBuffer.stride * 4,
                           outbuffer + h * rgbFrame->linesize[0],
                           rgbFrame->linesize[0]);
                }
                switch (avFrame->pict_type) {
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
            ANativeWindow_unlockAndPost(nativeWindow);

        }

    }

    avformat_free_context(mAvFormatContext);

    return 0;
}

int FFmpegVideoPlay::initFormatContext() {
    mAvFormatContext = avformat_alloc_context();
    //1.打开文件
    ret = avformat_open_input(&mAvFormatContext, mInputUrl, NULL, NULL);
    if (ret < 0) {
        LOGD("Couldn't open input stream. ret:%d", ret);
        return ret;
    }
    LOGD("open inputFile:%s", mInputUrl);
    //2.查找文件流信息
    ret = avformat_find_stream_info(mAvFormatContext, NULL);
    if (ret < 0) {
        LOGD("Couldn't find stream info. ret:%d", ret);
        return ret;
    }
    return ret;
}

int FFmpegVideoPlay::initFFmpegCodec() {
    return 0;
}

int FFmpegVideoPlay::initFFmeg(JNIEnv *env, jobject surface, const char *inputUrl) {
    mEnv = env;
    androidSurface = surface;
    mInputUrl = inputUrl;
    initFormatContext();
    return 0;
}
