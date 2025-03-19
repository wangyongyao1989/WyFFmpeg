package com.wangyongyao.h265.nal;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.media.MediaMuxer;
import android.util.Log;

import java.io.FileInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2025/3/17 11:34
 * Descibe : MyyFFmpeg com.wangyongyao.h265
 */
public class H265ToMp4Converter {

    private static final String TAG = "HevcToMp4Converter";
    private static final String MIME_TYPE_HEVC = "video/hevc";
    private static final int NAL_UNIT_TYPE_VPS = 32;
    private static final int NAL_UNIT_TYPE_SPS = 33;
    private static final int NAL_UNIT_TYPE_PPS = 34;
    private static final int NAL_UNIT_TYPE_IDR_N_LP = 19;
    private static final int NAL_UNIT_TYPE_IDR_W_RADL = 20;

    private static boolean isMuxerStarted = false;

    public static void convert(String inputPath, String outputPath) {
        MediaMuxer muxer = null;
        FileInputStream fis = null;
        try {
            // 1. 读取H.265裸流文件
            fis = new FileInputStream(inputPath);
            byte[] rawData = new byte[fis.available()];
            fis.read(rawData);
            fis.close();

            // 2. 解析NAL单元
            List<NalUnit> nalUnits = parseNalUnits(rawData);
            Log.d(TAG, "Conversion nalUnits:" + nalUnits.size());

            // 3. 提取参数集并创建MediaFormat
            MediaFormat format = createMediaFormat(nalUnits);
            if (format == null) {
                throw new IllegalArgumentException("Failed to create MediaFormat: SPS/PPS missing");
            }

            // 4. 初始化MediaMuxer
            muxer = new MediaMuxer(outputPath, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);
            int trackIndex = muxer.addTrack(format);
            Log.d(TAG, "Conversion trackIndex:" + trackIndex);
            if (muxer != null && !isMuxerStarted) {
                muxer.start();
                isMuxerStarted = true;
            }

            // 5. 写入视频数据
            long pts = writeVideoData(muxer, trackIndex, nalUnits, format);

            Log.d(TAG, "Conversion completed successfully");
            MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
            bufferInfo.offset = 0;
            bufferInfo.size = 0;
            bufferInfo.flags = MediaCodec.BUFFER_FLAG_END_OF_STREAM;
            bufferInfo.presentationTimeUs = pts + (long) (1_000_000 / format.getInteger(MediaFormat.KEY_FRAME_RATE));
            muxer.writeSampleData(
                    trackIndex,
                    ByteBuffer.allocateDirect(0), bufferInfo);

        } catch (IOException e) {
            Log.e(TAG, "IO error: " + e.getMessage());
        } finally {
            if (muxer != null) {
                try {
                    if (isMuxerStarted) {
                        muxer.stop();
                    }
                } catch (IllegalStateException e) {
                    Log.e(TAG, "Failed to stop muxer: " + e.getMessage());
                }
                muxer.release();
            }
            if (fis != null) {
                try {
                    fis.close();
                } catch (IOException e) {
                    Log.e(TAG, "Failed to close input stream: " + e.getMessage());
                }
            }
        }
    }

    //-------------------------- 核心解析逻辑 --------------------------//

    /**
     * 解析NAL单元（支持3字节和4字节起始码）
     */
    private static List<NalUnit> parseNalUnits(byte[] data) {
        List<NalUnit> units = new ArrayList<>();
        int start = 0;
        while (start < data.length - 4) {
            if (start < 0) break;
            if (data[start] == 0x00 && data[start + 1] == 0x00) {
                int prefixLength = 0;
                if (data[start + 2] == 0x01) { // 3字节起始码 0x000001
                    prefixLength = 3;
                } else if (data[start + 2] == 0x00 && data[start + 3] == 0x01) { // 4字节起始码 0x00000001
                    prefixLength = 4;
                }

                if (prefixLength > 0) {
                    int nalStart = start + prefixLength;
                    int nalType = (data[nalStart] >> 1) & 0x3F; // 提取NAL类型

                    int nextStart = findNextStartCode(data, start + prefixLength);
                    int length = (nextStart == -1) ? data.length - nalStart : nextStart - nalStart;

                    byte[] nalData = new byte[length];
                    System.arraycopy(data, nalStart, nalData, 0, length);
                    units.add(new NalUnit(nalType, nalData));

                    start = nextStart;
                    continue;
                }
            }
            start++;
        }
        return units;
    }

    /**
     * 创建包含HEVC参数的MediaFormat
     */
    private static MediaFormat createMediaFormat(List<NalUnit> nalUnits) {
        ByteBuffer vps = null, sps = null, pps = null;

        for (NalUnit nal : nalUnits) {
            switch (nal.type) {
                case NAL_UNIT_TYPE_VPS:
                    vps = ByteBuffer.wrap(nal.data);
                    byte[] array = vps.array();
                    Log.d(TAG, "NAL_UNIT_TYPE_VPS ===:" + Arrays.toString(array));
                    break;
                case NAL_UNIT_TYPE_SPS:
                    sps = ByteBuffer.wrap(nal.data);
                    break;
                case NAL_UNIT_TYPE_PPS:
                    pps = ByteBuffer.wrap(nal.data);
                    break;
            }
        }

        if (sps == null || pps == null) {
            Log.e(TAG, "Critical parameters missing: SPS=" + (sps != null) + ", PPS=" + (pps != null));
            return null;
        }

        // 从SPS解析分辨率
        int[] resolution = parseResolution(sps);
        if (resolution == null) return null;

        // 创建MediaFormat
        MediaFormat format = MediaFormat.createVideoFormat(MIME_TYPE_HEVC, resolution[0], resolution[1]);


        // HEVC参数集
        ByteBuffer csd = ByteBuffer.allocate(vps.capacity() + sps.capacity() + pps.capacity() + 12);
        csd.put((byte) 0x01); // configurationVersion
        csd.put(vps);        // vps
        csd.put((byte) 0x01); // numOfSps
        csd.putShort((short) sps.capacity());
        csd.put(sps);        // sps
        csd.put((byte) 0x01); // numOfPps
        csd.putShort((short) pps.capacity());
        csd.put(pps);        // pps
        csd.flip();

        format.setByteBuffer("csd-0", csd);
        // 设置帧率（需从SPS解析）
        double frameRate = parseFrameRate(sps);
        format.setInteger(MediaFormat.KEY_FRAME_RATE, (int) frameRate);

        Log.d(TAG, String.format("Video format: %dx%d @ %.2f fps", resolution[0], resolution[1], frameRate));
        return format;
    }

    /**
     * 写入视频数据（含参数集和帧数据）
     */
    private static long writeVideoData(MediaMuxer muxer, int trackIndex, List<NalUnit> nalUnits, MediaFormat format) {
        ByteBuffer buffer = ByteBuffer.allocateDirect(4 * 1024 * 1024);
        long pts = 0;
        long frameDurationUs = (long) (1_000_000 / format.getInteger(MediaFormat.KEY_FRAME_RATE));

        // 先写入参数集（SPS/PPS）
        writeParameterSets(muxer, trackIndex, buffer, nalUnits);

        for (NalUnit nal : nalUnits) {
            if (isVideoData(nal.type)) {
                buffer.clear();
                buffer.putInt(0x000001); // 添加4字节起始码
                buffer.put(nal.data);
                buffer.flip();

                MediaCodec.BufferInfo info = new MediaCodec.BufferInfo();
                info.presentationTimeUs = pts;
                info.size = buffer.remaining();
                info.flags = (nal.type == NAL_UNIT_TYPE_IDR_N_LP || nal.type == NAL_UNIT_TYPE_IDR_W_RADL)
                        ? MediaCodec.BUFFER_FLAG_KEY_FRAME : 0;

                muxer.writeSampleData(trackIndex, buffer, info);
                pts += frameDurationUs;

                Log.v(TAG, "Wrote frame: type=" + nal.type + ", pts=" + info.presentationTimeUs);
            }
        }

        return pts;
    }

    //-------------------------- 工具方法 --------------------------//

    /**
     * 写入参数集（SPS/PPS）
     */
    private static void writeParameterSets(MediaMuxer muxer, int trackIndex, ByteBuffer buffer, List<NalUnit> nalUnits) {
        MediaCodec.BufferInfo info = new MediaCodec.BufferInfo();
        info.flags = MediaCodec.BUFFER_FLAG_CODEC_CONFIG;

        for (NalUnit nal : nalUnits) {
            if (nal.type == NAL_UNIT_TYPE_SPS || nal.type == NAL_UNIT_TYPE_PPS) {
                buffer.clear();
                buffer.putInt(0x000001);
                buffer.put(nal.data);
                buffer.flip();

                info.size = buffer.remaining();
                muxer.writeSampleData(trackIndex, buffer, info);
                Log.d(TAG, "Wrote parameter set: type=" + nal.type);
            }
        }
    }

    /**
     * 从SPS解析分辨率
     */
    private static int[] parseResolution(ByteBuffer sps) {
        try {
            BitStreamReader reader = new BitStreamReader(sps.duplicate());
            reader.readBits(16); // 跳过NAL头
            reader.readUE();     // sps_video_parameter_set_id
            int sps_max_sub_layers_minus1 = reader.readBits(3);// sps_max_sub_layers_minus1
            reader.readBit();    // temporal_id_nesting_flag
            parseProfileTierLevel(reader,sps_max_sub_layers_minus1); // 跳过profile_tier_level

            int width = reader.readUE();
            int height = reader.readUE();
            Log.e(TAG, "parseResolution width:" + width + ", height" + height);

            return new int[]{704, 576};
        } catch (Exception e) {
            Log.e(TAG, "Resolution parsing failed: " + e.getMessage());
            return null;
        }
    }

    /**
     * 从SPS解析帧率（简化实现）
     */
    private static double parseFrameRate(ByteBuffer sps) {
//        try {
//            BitStreamReader reader = new BitStreamReader(sps.duplicate());
//            // ... 跳转到vui_parameters部分解析time_scale和num_units_in_tick
//            return 30.0; // 示例值，需实际解析
//        } catch (Exception e) {
//            return 30.0; // 默认30fps
//        }
        return 25.00;
    }

    /**
     * 查找下一个起始码位置
     */
    private static int findNextStartCode(byte[] data, int start) {
        for (int i = start; i < data.length - 4; i++) {
            if (data[i] == 0x00 && data[i + 1] == 0x00 && data[i + 2] == 0x01) {
                return i;
            } else if (data[i] == 0x00 && data[i + 1] == 0x00 && data[i + 2] == 0x00 && data[i + 3] == 0x01) {
                return i;
            }
        }
        return -1;
    }

    private static boolean isVideoData(int nalType) {
        return nalType >= 0 && nalType <= 31
                && nalType != NAL_UNIT_TYPE_VPS
                && nalType != NAL_UNIT_TYPE_SPS
                && nalType != NAL_UNIT_TYPE_PPS;
    }

    //-------------------------- 辅助类 --------------------------//

    /**
     * NAL单元封装类
     */
    static class NalUnit {
        final int type;
        final byte[] data;

        NalUnit(int type, byte[] data) {
            this.type = type;
            this.data = data;
        }
    }

    /**
     * 位流读取工具（关键实现）
     */
    static class BitStreamReader {
        private final ByteBuffer buffer;
        private int currentByte;
        private int bitOffset = 8;

        BitStreamReader(ByteBuffer data) {
            this.buffer = data.duplicate().order(ByteOrder.BIG_ENDIAN);
            advanceByte();
        }

        int readBit() {
            if (bitOffset >= 8) advanceByte();
            return (currentByte >> (7 - (bitOffset++ % 8))) & 0x01;
        }

        int readBits(int n) {
            int value = 0;
            for (int i = 0; i < n; i++) {
                value = (value << 1) | readBit();
            }
            return value;
        }

        int readUE() {
            int leadingZeros = 0;
            while (readBit() == 0) leadingZeros++;
            return (1 << leadingZeros) - 1 + readBits(leadingZeros);
        }

        private void advanceByte() {
            if (buffer.hasRemaining()) {
                currentByte = buffer.get() & 0xFF;
                bitOffset = 0;
            } else {
                currentByte = 0;
                bitOffset = 8;
            }
        }
    }

    /**
     * 解析profile_tier_level（跳过不影响分辨率）
     */
    private static void parseProfileTierLevel(BitStreamReader reader, int maxSubLayers) {
        // 1. 通用配置
        int general_profile_space = reader.readBits(2);
        boolean general_tier_flag = reader.readBit() == 1;
        int general_profile_idc = reader.readBits(5);

        // 2. Profile 兼容性标志（32 bits）
        for (int i = 0; i < 32; i++) {
            reader.readBit();
        }

        // 3. 约束标志（6 bits）
        reader.readBit(); // general_progressive_source_flag
        reader.readBit(); // general_interlaced_source_flag
        reader.readBit(); // general_non_packed_constraint_flag
        reader.readBit(); // general_frame_only_constraint_flag
        reader.readBit(); // general_reserved_zero_44bits (跳过2位)
        reader.readBit();

        // 4. Level 信息
        int general_level_idc = reader.readBits(8);

        // 5. 子层信息（根据 maxSubLayers）
        for (int i = 0; i < maxSubLayers; i++) {
            reader.readBit(); // sub_layer_profile_present_flag
            reader.readBit(); // sub_layer_level_present_flag
        }

        // 6. 跳过保留位
        if (maxSubLayers > 0) {
            for (int i = maxSubLayers; i < 8; i++) {
                reader.readBits(2); // reserved_zero_2bits
            }
        }
    }
}
