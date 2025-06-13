package com.example.myyffmpeg.converter

import android.media.MediaCodec
import android.media.MediaCodecInfo
import android.media.MediaMuxer
import android.util.Log
import java.nio.ByteBuffer
import java.util.Arrays
import android.media.MediaFormat
import com.example.myyffmpeg.converter.VideoSaver.Companion
import com.example.myyffmpeg.utils.StringToIntConverter


class AndroidMediaMuxer(outputPath: String, outputFormat: Muxer.OutputFormat
                        , mediaFormat: MediaFormat1
): Muxer {

    private val mediaMuxer = MediaMuxer(outputPath, outputFormat.value)


    private var gotCSD = false
    private var videoTrackIndex: Int = -1
    private var finished: Boolean = false
    private val videoWidth: Int = mediaFormat.getInt(MediaFormat.KEY_WIDTH, 1280)
    private val videoHeight: Int = mediaFormat.getInt(MediaFormat.KEY_HEIGHT, 720)
    private val videoFrameRate: Int = mediaFormat.getInt(MediaFormat.KEY_FRAME_RATE, 30)
    private var frameIndex = 0


    companion object {
        private const val TAG = "AndroidMediaMuxer"
    }
    private var mime: String? = null

    init {
        val mimeType = mediaFormat.getString(MediaFormat.KEY_MIME)
        if(mimeType != null) {
            mime = if(mimeType.lowercase() == "h265") {
                MediaFormat.MIMETYPE_VIDEO_HEVC
            } else if (mimeType.lowercase() == "h264") {
                MediaFormat.MIMETYPE_VIDEO_AVC
            } else {
                mimeType
            }
        } else {
            throw IllegalArgumentException("mime type is null")
        }
    }

    private var csdFinder = CsdFinder(mime!!)



    override fun writeVideoSampleData(data: ByteArray, timestamp: Long, keyFrame: Boolean) {

        if(!gotCSD) {
            val found = csdFinder.findCSD(data)
            Log.e(TAG, "found: $found")
            if(found) {
                configMuxer(csdFinder.csd0!!, csdFinder.csd1, csdFinder.csd2)
                Log.e(TAG, "gotCSD = true: ")
                gotCSD = true
            }
            return
        }

        if (!finished) {
            val inputBuffer = ByteBuffer.wrap(data)
            val bufferInfo = MediaCodec.BufferInfo()
            bufferInfo.offset = 0
            bufferInfo.size = data.size
            bufferInfo.flags = 0
            if (isKeyFrame(data)) {
                bufferInfo.flags = MediaCodec.BUFFER_FLAG_KEY_FRAME
            }
            bufferInfo.presentationTimeUs = computePresentationTime(frameIndex++)

            // getInputBuffer() will fill the inputBuffer with one frame of encoded
            // sample from either MediaCodec or MediaExtractor, set isAudioSample to
            // true when the sample is audio data, set up all the fields of bufferInfo,
            // and return true if there are no more samples.
            Log.d(TAG, "create muxer for file:333333333 ");

            mediaMuxer.writeSampleData(videoTrackIndex, inputBuffer, bufferInfo)
        }
    }

    private fun isKeyFrame(nalUnit: ByteArray): Boolean {
        if(mime == MediaFormat.MIMETYPE_VIDEO_HEVC) {
            // 读取NAL头字节
            val nalHeader = nalUnit[4].toInt() and 0xFF

            // 提取nal_unit_type
            val nalUnitType = (nalHeader and 0x7E) shr 1


            // 判断是否是关键帧
            return nalUnitType in 16..32
        } else if (mime == MediaFormat.MIMETYPE_VIDEO_AVC) {
            //FIXME
            // 读取NAL头字节
            val nalHeader = nalUnit[4].toInt() and 0xFF

            // 提取nal_unit_type
            val nalUnitType = (nalHeader and 0x7E) shr 1


            // 判断是否是关键帧
            return nalUnitType >= 16 && nalUnitType <= 32
        }

        return false
    }

    private fun configMuxer(csd0: ByteArray, csd1: ByteArray? = null, csd2: ByteArray? = null) {
        val mediaFormat = MediaFormat.createVideoFormat(mime!!, videoWidth, videoHeight)
        mediaFormat.setInteger(
            MediaFormat.KEY_COLOR_FORMAT,
            MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Flexible
        )
        //        mediaFormat.setInteger(MediaFormat.KEY_BIT_RATE, BIT_RATE);
//        mediaFormat.setInteger(MediaFormat.KEY_FRAME_RATE, FRAME_RATE);
//        mediaFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, I_FRAME_INTERVAL);
        mediaFormat.setByteBuffer(
            "csd-0",
            ByteBuffer.wrap(csd0)
        ) // sps and pps have been retrieved from the stream's NAL 7/8
        if (csd1 != null) {
            mediaFormat.setByteBuffer(
                "csd-1",
                ByteBuffer.wrap(csd1)
            )
        }
        if (csd2 != null) {
            mediaFormat.setByteBuffer(
                "csd-2",
                ByteBuffer.wrap(csd2)
            )
        }

        //        mediaFormat.setLong(MediaFormat.KEY_DURATION, 10 * 1000);

        // 创建 MediaMuxer 封装器，用于将编码后的数据封装成 MP4 文件
        videoTrackIndex = mediaMuxer.addTrack(mediaFormat)
        mediaMuxer.start()
    }

    private fun computePresentationTime(frameIndex: Int): Long {
        return 42 + frameIndex * 1000000L / videoFrameRate
    }


    override fun release() {
        finished = true
        frameIndex = 0
        try {
            mediaMuxer.stop()
        } catch (e: Exception) {
            Log.e("stop muxer error: ${e.message}", TAG)
        }
        try {
            mediaMuxer.release()
        } catch (e: Exception) {
            Log.e("release muxer error: ${e.message}", TAG)
        }
    }
}



/**
 * https://developer.android.com/reference/android/media/MediaCodec#CSD
 */
class CsdFinder(private val mimeType: String) {
    var csd0: ByteArray? = null
    var csd1: ByteArray? = null
    var csd2: ByteArray? = null

    companion object {
        private const val TAG = "CsdFinder"
    }

    fun findCSD(data: ByteArray): Boolean {
//        Log.d("findCSD, mimeType: $mimeType", TAG)
        if(mimeType == MediaFormat.MIMETYPE_VIDEO_HEVC) {
            val result = findHevcCsd(data)
            Log.e(TAG, "result: $result")

//            Log.d("${mimeType}, findCSD result: ${result.contentToString()}", TAG)
            return result != null
        } else if (mimeType == MediaFormat.MIMETYPE_VIDEO_AVC) {
            val result = findAvcCsd(data)
//            Log.d("${mimeType}, findCSD result: $result", TAG)
            return result.isNotEmpty()
        }

        return false
    }


    private fun findAvcCsd(data: ByteArray): List<ByteArray> {
        //VPS、SPS、PPS
         if (isSPS(data)) {
             csd0 = Arrays.copyOfRange(data, 0, data.size)
         } else if (isPPS(data)) {
            if (csd0 != null) {
                csd1 = Arrays.copyOfRange(data, 0, data.size)
                return listOf(csd0!!, csd1!!)
            }
        }
        return emptyList()
    }

    /**
     * 对于 HEVC，需要找到 VPS、SPS、PPS，然后拼接成一个数组，作为 csd-0 传入。
     */
    private fun findHevcCsd(data: ByteArray): ByteArray? {
        //VPS、SPS、PPS
        val bytes2hex = StringToIntConverter.bytes2hex(data)
        Log.e(TAG, "findHevcCsd: $bytes2hex")
        Log.e(TAG, "data: ${data.size}")

        if (isVps(data)) {
            Log.e(TAG, "isVps Arrays.copyOfRang: ")

            csd0 = Arrays.copyOfRange(data, 0, data.size)
        } else if (isSPS(data)) {
            Log.e(TAG, "isSPS Arrays.copyOfRang: ")
            if (csd0 != null) {
                csd0 = arraySplice(csd0, data)
            }
        } else if (isPPS(data)) {
            if (csd0 != null) {
                Log.e(TAG, "arraySplice isPPS: ")
                csd0 = arraySplice(csd0, data)
                return csd0
            }
        }
        return null
    }



    private fun arraySplice(src1: ByteArray?, src2: ByteArray?): ByteArray {
        require(!(src1 == null || src2 == null)) { "source byte can't be null !" }
        val len1 = src1.size
        val len2 = src2.size
        val copy = ByteArray(len1 + len2)
        System.arraycopy(src1, 0, copy, 0, len1)
        System.arraycopy(src2, 0, copy, len1, len2)
        return copy
    }


    private fun isVps(nalUnit: ByteArray): Boolean {
        var index = 0
        // 查找起始码 (3 字节或 4 字节)
        index += if (nalUnit[index].toInt() == 0x00
            && nalUnit[index + 1].toInt() == 0x00
            && nalUnit[index + 2].toInt() == 0x01) {
            3 // 起始码长度为 3 字节
        } else if (nalUnit[index].toInt() == 0x00
            && nalUnit[index + 1].toInt() == 0x00
            && nalUnit[index + 2].toInt() == 0x00
            && nalUnit[index + 3].toInt() == 0x01) {
            4 // 起始码长度为 4 字节
        } else {
            return false
        }

        // 读取NAL头字节
        val nalHeader = nalUnit[index].toInt() and 0xFF
        Log.e(TAG, "isVps nalHeader: $nalHeader")

        // 提取nal_unit_type
        val nalUnitType = (nalHeader and 0x7E) shr 1
        Log.e(TAG, "isVps nalUnitType: $nalUnitType")

        return nalUnitType == 32
    }

    private fun isSPS(nalUnit: ByteArray): Boolean {
        var index = 0
        // 查找起始码 (3 字节或 4 字节)
        index += if (nalUnit[index].toInt() == 0x00
            && nalUnit[index + 1].toInt() == 0x00
            && nalUnit[index + 2].toInt() == 0x01) {
            3 // 起始码长度为 3 字节
        } else if (nalUnit[index].toInt() == 0x00
            && nalUnit[index + 1].toInt() == 0x00
            && nalUnit[index + 2].toInt() == 0x00
            && nalUnit[index + 3].toInt() == 0x01) {
            4 // 起始码长度为 4 字节
        } else {
            return false
        }

        // 读取NAL头字节
        val nalHeader = nalUnit[index].toInt() and 0xFF
        Log.e(TAG, "isSPS nalHeader: $nalHeader")

        when (mimeType) {
            MediaFormat.MIMETYPE_VIDEO_HEVC -> {
                // 提取nal_unit_type
                val nalUnitType = (nalHeader and 0x7E) shr 1
                Log.e(TAG, "isSPS nalUnitType: $nalUnitType")

                return nalUnitType == 33
            }
            MediaFormat.MIMETYPE_VIDEO_AVC -> {
                val nalUnitType = nalHeader and 0x1F
                return nalUnitType == 7
            }
            else -> {
                return false
            }
        }
    }

    private fun isPPS(nalUnit: ByteArray): Boolean {
        var index = 0
        // 查找起始码 (3 字节或 4 字节)
        index += if (nalUnit[index].toInt() == 0x00
            && nalUnit[index + 1].toInt() == 0x00
            && nalUnit[index + 2].toInt() == 0x01) {
            3 // 起始码长度为 3 字节
        } else if (nalUnit[index].toInt() == 0x00
            && nalUnit[index + 1].toInt() == 0x00
            && nalUnit[index + 2].toInt() == 0x00
            && nalUnit[index + 3].toInt() == 0x01) {
            4 // 起始码长度为 4 字节
        } else {
            return false
        }

        // 读取NAL头字节
        val nalHeader = nalUnit[index].toInt() and 0xFF

        when (mimeType) {
            MediaFormat.MIMETYPE_VIDEO_HEVC -> {
                // 提取nal_unit_type
                Log.e(TAG, "nalHeader: $nalHeader")
                val nalUnitType = (nalHeader and 0x7E) shr 1
                Log.e(TAG, "isPPS: $nalUnitType")
                return nalUnitType == 34
            }
            MediaFormat.MIMETYPE_VIDEO_AVC -> {
                val nalUnitType = nalHeader and 0x1F
                return nalUnitType == 8
            }
            else -> {
                return false
            }
        }

    }
}