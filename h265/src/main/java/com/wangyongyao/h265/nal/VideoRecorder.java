package com.wangyongyao.h265.nal;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.media.MediaMuxer;
import android.os.Environment;
import android.util.Log;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.Arrays;

/**
 * https://stackoverflow.com/questions/53637969/mediamuxer-writing-h264-stream-to-mpg-file
 */
public class VideoRecorder {
    private static final String OUTPUT_FILE_PATH = Environment.getExternalStorageDirectory() + "/output.mp4";
    private static final String MIME_TYPE = MediaFormat.MIMETYPE_VIDEO_HEVC;
    private static final int WIDTH = 704;
    private static final int HEIGHT = 576;
    private static final int FRAME_RATE = 25;
    private static final int BIT_RATE = 1570;
    private static final int I_FRAME_INTERVAL = 4;

    private MediaMuxer mediaMuxer;

    int videoTrackIndex = -1;
    boolean finished = false;

    private boolean isGetCsd = false;


    public VideoRecorder(String path) {
        initMuxer(path);
    }

    private static final String TAG = "VideoRecorder";


    private void initMuxer(String path) {
        try {
            // 创建 MediaMuxer 封装器，用于将编码后的数据封装成 MP4 文件
            mediaMuxer = new MediaMuxer(path, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

    }

    private void configMuxer(byte[] csd) {
        MediaFormat mediaFormat = MediaFormat.createVideoFormat(MIME_TYPE, WIDTH, HEIGHT);
        mediaFormat.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Flexible);
        mediaFormat.setInteger(MediaFormat.KEY_BIT_RATE, BIT_RATE);
        mediaFormat.setInteger(MediaFormat.KEY_FRAME_RATE, FRAME_RATE);
        mediaFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, I_FRAME_INTERVAL);
        mediaFormat.setByteBuffer("csd-0", ByteBuffer.wrap(csd)); // sps and pps have been retrieved from the stream's NAL 7/8
//        mediaFormat.setLong(MediaFormat.KEY_DURATION, 10 * 1000);

        // 创建 MediaMuxer 封装器，用于将编码后的数据封装成 MP4 文件
        isGetCsd = true;
        videoTrackIndex = mediaMuxer.addTrack(mediaFormat);
        mediaMuxer.start();
    }


    private final Object mLock = new Object();


    private long frameIndex = 0;

    private void encode(byte[] h265Data) {
        if (!isGetCsd) {
            boolean isVps = isVps(h265Data);
            if (isVps) {
                byte[] csd = Arrays.copyOfRange(h265Data, 0, 84);
                configMuxer(csd);
            }
            return;
        }

        if (!finished) {
            ByteBuffer inputBuffer = ByteBuffer.wrap(h265Data);
            MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
            bufferInfo.offset = 0;
            bufferInfo.size = h265Data.length;
            bufferInfo.flags = 0;
            if (isKeyFrame(h265Data)) {
                bufferInfo.flags |= MediaCodec.BUFFER_FLAG_KEY_FRAME;
            }
            bufferInfo.presentationTimeUs = computePresentationTime(frameIndex++);

            // getInputBuffer() will fill the inputBuffer with one frame of encoded
            // sample from either MediaCodec or MediaExtractor, set isAudioSample to
            // true when the sample is audio data, set up all the fields of bufferInfo,
            // and return true if there are no more samples.
            mediaMuxer.writeSampleData(videoTrackIndex, inputBuffer, bufferInfo);
            Log.e(TAG, "writeSampleData: " + h265Data.length);
        }
    }

    public void save(byte[] h265Data) {
        synchronized (mLock) {
            encode(h265Data);
        }
    }

    public boolean isKeyFrame(byte[] nalUnit) {
        // 读取NAL头字节
        int nalHeader = nalUnit[4] & 0xFF;
        // 提取nal_unit_type
        int nalUnitType = (nalHeader & 0x7E) >> 1;
        // 判断是否是关键帧
        return nalUnitType >= 16 && nalUnitType <= 32;
    }


    public boolean isVps(byte[] nalUnit) {
        // 读取NAL头字节
        int nalHeader = nalUnit[4] & 0xFF;
        // 提取nal_unit_type
        int nalUnitType = (nalHeader & 0x7E) >> 1;
        if (nalUnitType == 32) {
            return true;
        } else {
            return false;
        }
    }

    public void release() {
        finished = true;
        if (videoTrackIndex < 0) return;
        MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
        bufferInfo.offset = 0;
        bufferInfo.size = 0;
        bufferInfo.flags = MediaCodec.BUFFER_FLAG_END_OF_STREAM;
        bufferInfo.presentationTimeUs = computePresentationTime(frameIndex);
        mediaMuxer.writeSampleData(
                videoTrackIndex,
                ByteBuffer.allocateDirect(0), bufferInfo);
        Log.e(TAG, "mediaMuxer.stop()========================== " );

        // 释放资源
        mediaMuxer.stop();
        mediaMuxer.release();
    }

    private static long computePresentationTime(long frameIndex) {
        long frameIndexTime = frameIndex * 1000000 / FRAME_RATE;
        long time = 42 + frameIndexTime;
        return time;
    }


}
