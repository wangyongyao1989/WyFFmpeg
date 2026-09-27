package com.example.myyffmpeg.converter

import android.media.MediaCodec
import android.media.MediaCodecInfo
import android.media.MediaMuxer
import android.util.Log
import java.nio.ByteBuffer
import java.util.Arrays
import android.media.MediaFormat


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
            if(csdFinder.findCSD(data)) {
                configMuxer(csdFinder.csd0!!, csdFinder.csd1, csdFinder.csd2)
                gotCSD = true
            }
            return
        }

        if (!finished) {
            // Android 10 MPEG4Writer 对 AVC/HEVC 样本走 addMultipleLengthPrefixedSamples_l：
            // StripStartcode 去掉首个 4 字节起始码后，getNextNALUnit 按 "00 00 0x 01" 边界
            // 切分并以 "-4" 计算 NAL 长度，即要求 NAL 之间必须是 4 字节起始码的 AnnexB 流。
            // 若喂 3 字节起始码（边界偏 1 字节）或 AVCC 长度前缀（被当数据扫描、
            // 撞进切片里的 00 00 01），会产出垃圾长度触发 addLengthPrefixedSample_l
            // 的 length+4 ubsan add-overflow 崩溃。
            val annexb = normalizeTo4ByteStartCodes(data)
            val inputBuffer = ByteBuffer.wrap(annexb)
            val bufferInfo = MediaCodec.BufferInfo()
            bufferInfo.offset = 0
            bufferInfo.size = annexb.size
            bufferInfo.flags = 0
            if (keyFrame || isKeyFrame(data)) {
                bufferInfo.flags = MediaCodec.BUFFER_FLAG_KEY_FRAME
            }
            // 优先使用调用方给的真实时间戳，缺省(0)时按帧号回退
            bufferInfo.presentationTimeUs =
                if (timestamp > 0) timestamp else computePresentationTime(frameIndex++)

            mediaMuxer.writeSampleData(videoTrackIndex, inputBuffer, bufferInfo)
        }
    }

    /** 把所有 3 字节起始码规范化为 4 字节（00 00 01 → 00 00 00 01），已是 4 字节的保持不变 */
    private fun normalizeTo4ByteStartCodes(data: ByteArray): ByteArray {
        val out = java.io.ByteArrayOutputStream(data.size + 8)
        var i = 0
        while (i < data.size) {
            if (i + 2 < data.size && data[i] == 0.toByte() && data[i + 1] == 0.toByte()
                && data[i + 2] == 1.toByte()
            ) {
                if (i > 0 && data[i - 1] == 0.toByte()) {
                    // 前面已有第 4 个 0，属于 4 字节起始码，照常输出
                    out.write(data[i].toInt()); out.write(data[i + 1].toInt()); out.write(data[i + 2].toInt())
                } else {
                    out.write(0); out.write(0); out.write(0); out.write(1)
                }
                i += 3
            } else {
                out.write(data[i].toInt())
                i++
            }
        }
        return out.toByteArray()
    }

    /** 返回 AnnexB NAL 载荷起始下标（3 或 4 字节起始码之后），识别失败返回 -1 */
    private fun nalPayloadIndex(nalUnit: ByteArray): Int {
        if (nalUnit.size < 4) return -1
        return when {
            nalUnit[0].toInt() == 0 && nalUnit[1].toInt() == 0
                    && nalUnit[2].toInt() == 0 && nalUnit[3].toInt() == 1 -> 4
            nalUnit[0].toInt() == 0 && nalUnit[1].toInt() == 0
                    && nalUnit[2].toInt() == 1 -> 3
            else -> -1
        }
    }

    private fun isKeyFrame(nalUnit: ByteArray): Boolean {
        val index = nalPayloadIndex(nalUnit)
        if (index < 0 || index >= nalUnit.size) {
            return false
        }
        // 读取NAL头字节
        val nalHeader = nalUnit[index].toInt() and 0xFF
        return if (mime == MediaFormat.MIMETYPE_VIDEO_HEVC) {
            // 提取nal_unit_type，IRAP(16~21) 为关键帧
            val nalUnitType = (nalHeader and 0x7E) shr 1
            nalUnitType in 16..21
        } else if (mime == MediaFormat.MIMETYPE_VIDEO_AVC) {
            // AVC: nal_unit_type = header & 0x1F，5 = IDR
            val nalUnitType = nalHeader and 0x1F
            nalUnitType == 5
        } else {
            false
        }
    }

    private fun configMuxer(csd0: ByteArray, csd1: ByteArray? = null, csd2: ByteArray? = null) {
        // Android 10 框架 convertMessageToMetaData() 对 HEVC csd-0 调 reassembleHVCC()：
        // 要求首个 NAL 前是 4 字节起始码、NAL 之间也必须是 4 字节起始码
        // （3 字节会让扫描点落在 4 字节码中间，切出带前导 0x00 的 NAL，
        //   HevcParameterSets::makeHvcc 随即失败 → stop 时报 "Missing codec specific data"）。
        // 尾部残留 0x00 也会被 makeHvcc 判为非法（其校验 RBSP 末字节非 0）。
        val hevcCsd: ByteArray? = if (mime == MediaFormat.MIMETYPE_VIDEO_HEVC) {
            normalizeCsd0ForHVCCReassembly(csd0)
        } else null

        val mediaFormat = MediaFormat.createVideoFormat(mime!!, videoWidth, videoHeight)
        mediaFormat.setInteger(
            MediaFormat.KEY_COLOR_FORMAT,
            MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Flexible
        )
        if (hevcCsd != null) {
            mediaFormat.setByteBuffer("csd-0", ByteBuffer.wrap(hevcCsd))
        } else {
            mediaFormat.setByteBuffer("csd-0", ByteBuffer.wrap(csd0))
            if (csd1 != null) {
                mediaFormat.setByteBuffer("csd-1", ByteBuffer.wrap(csd1))
            }
            if (csd2 != null) {
                mediaFormat.setByteBuffer("csd-2", ByteBuffer.wrap(csd2))
            }
        }

        // 创建 MediaMuxer 封装器，用于将编码后的数据封装成 MP4 文件
        videoTrackIndex = mediaMuxer.addTrack(mediaFormat)
        mediaMuxer.start()
    }

    /**
     * csd-0（VPS+SPS+PPS 拼接）规范化：3 字节起始码全部升级为 4 字节，去掉尾部填充 0x00。
     * 返回 null 表示结构不符合预期，交回原 csd-0 走兜底路径。
     */
    private fun normalizeCsd0ForHVCCReassembly(csd0: ByteArray): ByteArray? {
        val nals = ArrayList<ByteArray>()
        var i = 0
        while (i < csd0.size) {
            val sc = when {
                i + 3 < csd0.size && csd0[i] == 0.toByte() && csd0[i + 1] == 0.toByte()
                        && csd0[i + 2] == 0.toByte() && csd0[i + 3] == 1.toByte() -> 4
                i + 2 < csd0.size && csd0[i] == 0.toByte() && csd0[i + 1] == 0.toByte()
                        && csd0[i + 2] == 1.toByte() -> 3
                else -> return null
            }
            var j = i + sc
            while (j + 2 < csd0.size) {
                if (csd0[j] == 0.toByte() && csd0[j + 1] == 0.toByte() && csd0[j + 2] == 1.toByte()) break
                j++
            }
            if (j + 2 >= csd0.size) j = csd0.size
            if (j > i + sc) nals.add(csd0.copyOfRange(i + sc, j))
            i = j
        }
        if (nals.size < 3) return null
        var end = nals.size - 1
        while (end >= 0 && nals[end].all { it == 0.toByte() }) end--   // 去掉全 0 的伪 NAL
        val out = java.io.ByteArrayOutputStream(csd0.size + 8)
        for (k in 0..end) {
            var len = nals[k].size
            while (len > 1 && nals[k][len - 1] == 0.toByte()) len--   // 去掉边界切分残留的 0x00
            out.write(0); out.write(0); out.write(0); out.write(1)
            out.write(nals[k], 0, len)
        }
        return out.toByteArray()
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

    /** HEVC 参数集是否已集齐 VPS/SPS/PPS */
    private var hasVps = false
    private var hasSps = false
    private var hasPps = false

    fun findCSD(data: ByteArray): Boolean {
        if (mimeType == MediaFormat.MIMETYPE_VIDEO_HEVC) {
            return findHevcCsd(data) != null
        } else if (mimeType == MediaFormat.MIMETYPE_VIDEO_AVC) {
            return findAvcCsd(data).isNotEmpty()
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
        if (isVps(data)) {
            hasVps = true
            csd0 = data.copyOf()
        } else if (isSPS(data)) {
            hasSps = true
            if (csd0 != null) {
                csd0 = arraySplice(csd0, data)
            }
        } else if (isPPS(data)) {
            hasPps = true
            if (csd0 != null) {
                csd0 = arraySplice(csd0, data)
                // 只有 VPS+SPS+PPS 全部集齐才算找到 csd（框架据此重组 hvcC）
                if (hasVps && hasSps && hasPps) {
                    return csd0
                }
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


    /** 返回起始码后的 NAL 头下标；不是图像 NAL 类型或太短返回 -1 */
    private fun nalHeaderIndex(nalUnit: ByteArray): Int {
        if (nalUnit.size < 5) return -1     // 起始码 + NAL 头至少 5 字节，防越界
        return when {
            nalUnit[0].toInt() == 0x00 && nalUnit[1].toInt() == 0x00
                    && nalUnit[2].toInt() == 0x00 && nalUnit[3].toInt() == 0x01 -> 4
            nalUnit[0].toInt() == 0x00 && nalUnit[1].toInt() == 0x00
                    && nalUnit[2].toInt() == 0x01 -> 3
            else -> -1
        }
    }

    /** NAL 类型：HEVC 取 (b0&0x7E)>>1，AVC 取 b0&0x1F */
    private fun nalType(nalUnit: ByteArray, index: Int): Int {
        val b = nalUnit[index].toInt() and 0xFF
        return if (mimeType == MediaFormat.MIMETYPE_VIDEO_HEVC) (b and 0x7E) shr 1
        else b and 0x1F
    }

    private fun isVps(nalUnit: ByteArray): Boolean {
        val index = nalHeaderIndex(nalUnit)
        if (index < 0 || mimeType != MediaFormat.MIMETYPE_VIDEO_HEVC) return false
        return nalType(nalUnit, index) == 32
    }

    private fun isSPS(nalUnit: ByteArray): Boolean {
        val index = nalHeaderIndex(nalUnit)
        if (index < 0) return false
        return nalType(nalUnit, index) == if (mimeType == MediaFormat.MIMETYPE_VIDEO_HEVC) 33 else 7
    }

    private fun isPPS(nalUnit: ByteArray): Boolean {
        val index = nalHeaderIndex(nalUnit)
        if (index < 0) return false
        return nalType(nalUnit, index) == if (mimeType == MediaFormat.MIMETYPE_VIDEO_HEVC) 34 else 8
    }
}