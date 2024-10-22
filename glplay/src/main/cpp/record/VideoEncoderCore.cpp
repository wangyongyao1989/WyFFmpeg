//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2024/10/15.
//

#include "../includeopengl/VideoEncoderCore.h"


VideoEncoderCore::VideoEncoderCore(size_t width, size_t height, size_t bitRate,
                                   const char *outPutFile) {
    LOGD("VideoEncoderCore- width: %d, height: %d, bitRate: %d ,outPutFile: %s", width, height,
         bitRate, outPutFile);
    m_MediaMuxer_fp = fopen(outPutFile, "wb+");// 打开新建一个文件。
    if (m_MediaMuxer_fp == nullptr) {
        LOGE("MediaCodecMuxer:: Mp4 file fopen err!");
        return;
    }

    // 由于muexr的原因，这里需要转换一下。
    m_MediaMuxer_fd = fileno(m_MediaMuxer_fp);
    if (m_MediaMuxer_fd < 0) {
        perror("mp4 file err: ");
        LOGE("MediaCodecMuxer:: Mp4 file open err! = %d", m_MediaMuxer_fd);
    }
    m_AMediaFormat = AMediaFormat_new();
    // H.264 Advanced Video Coding
    AMediaFormat_setString(m_AMediaFormat, AMEDIAFORMAT_KEY_MIME, MIME_TYPE);
    AMediaFormat_setInt32(m_AMediaFormat, AMEDIAFORMAT_KEY_WIDTH, width);
    AMediaFormat_setInt32(m_AMediaFormat, AMEDIAFORMAT_KEY_HEIGHT, height);
    AMediaFormat_setInt32(m_AMediaFormat, AMEDIAFORMAT_KEY_COLOR_FORMAT, COLOR_FormatSurface);
    AMediaFormat_setInt32(m_AMediaFormat, AMEDIAFORMAT_KEY_BIT_RATE, bitRate);
    // 30fps
    AMediaFormat_setInt32(m_AMediaFormat, AMEDIAFORMAT_KEY_FRAME_RATE, 30);
    // 5 seconds between I-frames
    AMediaFormat_setInt32(m_AMediaFormat, AMEDIAFORMAT_KEY_I_FRAME_INTERVAL, 5);

    m_AMediaCodec = AMediaCodec_createEncoderByType(MIME_TYPE);
    if (m_AMediaCodec == NULL) {
        LOGE("ERROR: AMediaCodec_createEncoderByType");
        return;
    }

    media_status_t configureStatus = AMediaCodec_configure(m_AMediaCodec, m_AMediaFormat, NULL,
                                                           NULL,
                                                           AMEDIACODEC_CONFIGURE_FLAG_ENCODE);
    if (configureStatus != AMEDIA_OK) {
        LOGE("ERROR: AMediaCodec_createEncoderByType");
        return;
    }
    //创建一个录制的WindowSurface，用于视频的录制
    media_status_t createInputSurfaceStatus = AMediaCodec_createInputSurface(m_AMediaCodec,
                                                                             &m_Encoder_WindowSurface);
    if (createInputSurfaceStatus != AMEDIA_OK) {
        LOGE("ERROR: AMediaCodec_createInputSurface :%d", createInputSurfaceStatus);
        return;
    }

    media_status_t codecStart = AMediaCodec_start(m_AMediaCodec);
    if (codecStart != AMEDIA_OK) {
        LOGE("ERROR: AMediaCodec_start");
        return;
    }

    // 新建一个复合输出
    m_AMediaMuxer = AMediaMuxer_new(m_MediaMuxer_fd, AMEDIAMUXER_OUTPUT_FORMAT_MPEG_4);
    LOGD(" AMediaMuxer_new OK");

    mTrackIndex = -1;
    mMuxerStarted = false;

}

VideoEncoderCore::~VideoEncoderCore() {
    release();
}


void VideoEncoderCore::drainEncoder(bool endOfStream) {
//    LOGE("drainEncoder thread:%ld", pthread_self());
    if (endOfStream) {
        LOGE("sending EOS to encoder");
        AMediaCodec_signalEndOfInputStream(m_AMediaCodec);
        return;
    }

    while (true) {
        AMediaCodecBufferInfo info;
        //time out usec 1
        ssize_t status = AMediaCodec_dequeueOutputBuffer(m_AMediaCodec, &info, 1);
//        LOGW("AMediaCodec_dequeueOutputBuffer status %d", status);

        if (status == AMEDIACODEC_INFO_TRY_AGAIN_LATER) {

            if (!endOfStream) {
                break;
            } else {
                LOGI("video no output available, spinning to await EOS");
            }
        } else if (status == AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED) {
            // not expected for an encoder
        } else if (status == AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED) {
            if (mMuxerStarted) {
                LOGW("format changed twice");
            }

            AMediaFormat *fmt = AMediaCodec_getOutputFormat(m_AMediaCodec);
            const char *s = AMediaFormat_toString(fmt);
            LOGI("video output format %s", s);

            mTrackIndex = AMediaMuxer_addTrack(m_AMediaMuxer, fmt);

            if (mTrackIndex != -1) {

                LOGI("AMediaMuxer_start");
                AMediaMuxer_start(m_AMediaMuxer);
                mMuxerStarted = true;
            }

        } else {
            uint8_t *encodeData = AMediaCodec_getOutputBuffer(m_AMediaCodec, status,
                                                              NULL/* out_size */);
            if (encodeData == NULL) {
                LOGE("encoder output buffer was null");
            }
            if ((info.flags & AMEDIACODEC_BUFFER_FLAG_CODEC_CONFIG) != 0) {
                LOGI("ignoring AMEDIACODEC_BUFFER_FLAG_CODEC_CONFIG");
                info.size = 0;
            }

            size_t dataSize = info.size;

            if (dataSize != 0) {

                if (!mMuxerStarted) {
                    LOGE("muxer has't started");
                }
//                info.presentationTimeUs = frameIndex * 1000000L / frameRate;
//                LOGI("AMediaMuxer_writeSampleData video size %d", dataSize);
                AMediaMuxer_writeSampleData(m_AMediaMuxer, mTrackIndex, encodeData, &info);
            } else {
                LOGI("Info emptye %d", dataSize);
            }

            AMediaCodec_releaseOutputBuffer(m_AMediaCodec, status, false);

            if ((info.flags & AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM) != 0) {

                if (!endOfStream) {
                    LOGW("reached end of stream unexpectly");
                } else {
                    LOGI("video end of stream reached");
                }
                break;
            }
        }
    }


}

void VideoEncoderCore::release() {
    if (m_AMediaCodec != nullptr) {
        AMediaCodec_stop(m_AMediaCodec);
    }
    if (m_AMediaMuxer != nullptr) {
        AMediaMuxer_stop(m_AMediaMuxer);
    }

    if (m_AMediaCodec != nullptr) {
        AMediaCodec_delete(m_AMediaCodec);
        m_AMediaCodec = nullptr;
    }
    if (m_AMediaMuxer != nullptr) {
        AMediaMuxer_delete(m_AMediaMuxer);
        m_AMediaMuxer = nullptr;
    }

    if (m_MediaMuxer_fp != nullptr) {
        delete m_MediaMuxer_fp;
        m_MediaMuxer_fp = nullptr;
    }
}

ANativeWindow *VideoEncoderCore::getInputSurface() {
    return m_Encoder_WindowSurface;
}



