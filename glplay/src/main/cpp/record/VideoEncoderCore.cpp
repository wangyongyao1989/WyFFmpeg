//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2024/10/15.
//

#include "../includeopengl/VideoEncoderCore.h"


VideoEncoderCore::VideoEncoderCore(size_t width, size_t height, size_t bitRate,
                                   const char *outPutFile) {
    LOGD("VideoEncoderCore- width: %d, height: %d, bitRate: %d", width, height, bitRate);
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

    media_status_t codecStart = AMediaCodec_start(m_AMediaCodec);
    if (codecStart != AMEDIA_OK) {
        LOGE("ERROR: AMediaCodec_start");
        return;
    }


    // 新建一个复合输出
    m_AMediaMuxer = AMediaMuxer_new(m_MediaMuxer_fd, AMEDIAMUXER_OUTPUT_FORMAT_MPEG_4);

    mTrackIndex = -1;
    mMuxerStarted = false;

}

VideoEncoderCore::~VideoEncoderCore() {

}


void VideoEncoderCore::drainEncoder(bool endOfStream) {

    if (endOfStream) {
        LOGE("sending EOS to encoder");
        AMediaCodec_signalEndOfInputStream(m_AMediaCodec);
    }

//    uint8_t *outputBuffer = AMediaCodec_getOutputBuffer(m_AMediaCodec, &bufferInfo, TIMEOUT);
    while (true) {
        ssize_t encoderStatus = AMediaCodec_dequeueOutputBuffer(m_AMediaCodec, &bufferInfo,
                                                                TIMEOUT);
        if (encoderStatus == AMEDIACODEC_INFO_TRY_AGAIN_LATER) {
            // no output available yet
            if (!endOfStream) {
                return;
                //break;      // out of while
            }
            if (endOfStream) {
                LOGE("no output available, spinning to await EOS");
                return;
            }

        } else if (encoderStatus == AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED) {
            // not expected for an encoder

        } else if (encoderStatus == AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED) {
            // should happen before receiving buffers, and should only happen once
            if (mMuxerStarted) {
                LOGE("ERROR: format changed twice");
            }
            AMediaFormat *newFormat = AMediaCodec_getOutputFormat(m_AMediaCodec);

            if (newFormat == nullptr) {
                LOGE("Unable to set new format.");
            }

            // now that we have the Magic Goodies, start the muxer
            mTrackIndex = AMediaMuxer_addTrack(m_AMediaMuxer, newFormat);
            media_status_t err = AMediaMuxer_start(m_AMediaMuxer);

            if (err != AMEDIA_OK) {
                LOGE("Error occurred:%d", err);
            }

            mMuxerStarted = true;
        } else if (encoderStatus < 0) {
            LOGE("unexpected result from encoder.dequeueOutputBuffer:%d", encoderStatus);
            // let's ignore it
        } else {

            size_t out_size;
            uint8_t *encodedData = AMediaCodec_getOutputBuffer(m_AMediaCodec, encoderStatus,
                                                               &out_size);

            if (out_size <= 0) {
                LOGE("Encoded data of size 0.");

            }

            if (encodedData == nullptr) {
                LOGE("encoderOutputBuffer  %d was null", encodedData);
            }


            if ((bufferInfo.flags & AMEDIACODEC_BUFFER_FLAG_CODEC_CONFIG) != 0) {
                // The codec config data was pulled out and fed to the muxer when we got
                // the INFO_OUTPUT_FORMAT_CHANGED status.  Ignore it.
                LOGE("ignoring BUFFER_FLAG_CODEC_CONFIG");

                bufferInfo.size = 0;
            }

            if (bufferInfo.size != 0) {
                if (!mMuxerStarted) {
                    LOGE("muxer hasn't started");
                }

                // adjust the ByteBuffer values to match BufferInfo (not needed?)
                //encodedData.position(mBufferInfo.offset);
                //encodedData.limit(mBufferInfo.offset + mBufferInfo.size);
                AMediaMuxer_writeSampleData(m_AMediaMuxer, mTrackIndex, encodedData, &bufferInfo);
            } else {
                LOGE("mBufferInfo empty");
            }

            AMediaCodec_releaseOutputBuffer(m_AMediaCodec, encoderStatus, false);

            if ((bufferInfo.flags & AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM) != 0) {
                if (!endOfStream) {
                    LOGE("reached end of stream unexpectedly");
                } else {
                    LOGE("end of stream reached");
                }
                break;      // out of while
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



