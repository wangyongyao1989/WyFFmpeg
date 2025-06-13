package com.example.myyffmpeg.converter;

/**
 * author: Feynman
 * date: 2024/11/12 18:13
 * description:
 */

interface Muxer {
    /**
     * 写入视频数据
     */
    fun writeVideoSampleData(data: ByteArray, timestamp: Long, keyFrame: Boolean)

    fun release()

    enum class OutputFormat(val value: Int) {


        /** MPEG4 media file format */
        MUXER_OUTPUT_MPEG_4(0),

        /** WEBM media file format */
        MUXER_OUTPUT_WEBM(1),

        /** 3GPP media file format */
        MUXER_OUTPUT_3GPP(2),

        /** HEIF media file format */
        MUXER_OUTPUT_HEIF(3),

        /** Ogg media file format */
        MUXER_OUTPUT_OGG(4)

    }
}