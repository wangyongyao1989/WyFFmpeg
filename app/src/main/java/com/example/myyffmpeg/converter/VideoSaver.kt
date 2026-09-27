package com.example.myyffmpeg.converter;

import android.util.Log
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update
import java.util.Calendar

/**
 * 使用方法
 * 1. 开始录制时创建VideoSaver对象，前提需要知道视频流基本参数：mimeType、width、height
 * 2. 当有视频帧数据来了后，通过VideoSaver.writeFrame方法写入一条完整NAL
 * 3. 停止录制时调用VideoSaver.stop方法，并销毁VideoSaver对象
 */

internal class VideoSaver(val fileDir: String, private val videoConfig: VideoConfig
                                        , private val outputFilePrefix: String) {

    /** 供 NAL 拆分器回调：type 为 HEVC nal_unit_type（无法识别时 -1） */
    fun interface NalSink {
        fun onNal(nal: ByteArray, type: Int, keyFrame: Boolean)
    }

    private var muxer: Muxer? = null

    private val _frameSaveState = MutableStateFlow(false)
    val frameSaveState = _frameSaveState.asStateFlow()

    private val maxFileSize = 500 * 1024 * 1024

    private var fileSize = 0L

    companion object {
        private const val TAG = "VideoSaver"
    }


    init {
        val outputPath = createOutputFile()
        createMuxer(outputPath)
    }

    private fun getCurrentDataTime(): DateTime {
        val calendar = Calendar.getInstance()
        val year = calendar.get(Calendar.YEAR)
        val month = calendar.get(Calendar.MONTH) + 1
        val day = calendar.get(Calendar.DAY_OF_MONTH)
        val hour = calendar.get(Calendar.HOUR_OF_DAY)
        val minute = calendar.get(Calendar.MINUTE)
        val second = calendar.get(Calendar.SECOND)
        val millisecond = calendar.get(Calendar.MILLISECOND)
        return DateTime(
            calendar.timeInMillis,
            year,
            month,
            day,
            hour,
            minute,
            second,
            millisecond
        )

    }

    private fun createOutputFile(): String {
        // 格式化日期
        val time = getCurrentDataTime()
        val formattedDate = "${time.year}${"%02d".format(time.monthNumber)}${"%02d".format(time.dayOfMonth)}" +
                "-${"%02d".format(time.hour)}${"%02d".format(time.minute)}${"%02d".format(time.second)}"
        val outputPath = "$fileDir/${outputFilePrefix}${formattedDate}.mp4"
        return outputPath
    }

    private fun createMuxer(outputPath: String) {
        Log.d(TAG, "create muxer for file: $outputPath")
        val mediaFormat1 = MediaFormat1.createVideoFormat(
            videoConfig.mimeType,
            videoConfig.width,
            videoConfig.height
        )
        muxer = AndroidMediaMuxer(outputPath, Muxer.OutputFormat.MUXER_OUTPUT_MPEG_4, mediaFormat1)
        Log.d(TAG, "create ===========: $muxer")

    }

    /**
     * 写入一条完整 NAL。
     * @return false 表示 muxer 已失效或写入异常，调用方应停止本次封装
     */
    fun writeFrame(nal: ByteArray, timestampUs: Long, keyFrame: Boolean): Boolean {
        val m = muxer ?: return false
        return try {
            if (fileSize > maxFileSize) {
                m.release()
                createMuxer(createOutputFile())
                fileSize = 0
            }
            muxer?.writeVideoSampleData(nal, timestampUs, keyFrame)
            fileSize += nal.size
            if (muxer != null) {
                _frameSaveState.update { true }
            }
            true
        } catch (e: Exception) {
            Log.e(TAG, "writeFrame error: ${e.message}")
            false
        }
    }

    fun stop() {
        Log.d(TAG, "stop video saver")
        if(muxer != null) {
            _frameSaveState.update { false }
        }
        muxer?.release()
        muxer = null
    }

}

data class DateTime(
    val currentTimeMillis: Long,
    val year: Int,
    val monthNumber: Int,
    val dayOfMonth: Int,
    val hour: Int,
    val minute: Int,
    val second: Int,
    val millisecond: Int
)

data class VideoConfig(var mimeType: String, var width: Int = 1280, var height: Int = 720) {
    companion object {
        const val MIME_H265 = "h265"
        const val MIME_HEVC = "hevc"
        const val MIME_H264 = "avc"
    }

    var useHardWare: Boolean = true
}