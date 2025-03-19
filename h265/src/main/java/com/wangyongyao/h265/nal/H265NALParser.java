package com.wangyongyao.h265.nal;

import android.util.Log;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;

/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2025/3/14 17:22
 * Descibe : MyyFFmpeg com.wangyongyao.h265
 */
public class H265NALParser {
    private static final String TAG = H265NALParser.class.getSimpleName();

    // 解析 VPS
    public static void parseVPS(byte[] buffer) {
        BitstreamReader11 reader = new BitstreamReader11(buffer);
        // 跳过 NAL Header 和 SPS 头部字段 



//        // 基本信息
//        int vps_video_parameter_set_id = reader.readBits(4);
//        int vps_base_layer_internal_flag = reader.readBit() ? 1 : 0;
//        int vps_base_layer_available_flag = reader.readBit() ? 1 : 0;
//        int vps_max_layers_minus1 = reader.readBits(6);
//        int vps_max_sub_layers_minus1 = reader.readBits(3);
//        int vps_temporal_id_nesting_flag = reader.readBit() ? 1 : 0;
//
//        Log.e(TAG,"vps_video_parameter_set_id: " + vps_video_parameter_set_id);
//        Log.e(TAG,"vps_base_layer_internal_flag: " + vps_base_layer_internal_flag);
//        Log.e(TAG,"vps_base_layer_available_flag: " + vps_base_layer_available_flag);
//        Log.e(TAG,"vps_max_layers_minus1: " + vps_max_layers_minus1);
//        Log.e(TAG,"vps_max_sub_layers_minus1: " + vps_max_sub_layers_minus1);
//        Log.e(TAG,"vps_temporal_id_nesting_flag: " + vps_temporal_id_nesting_flag);
//
//        // profile_tier_level 部分
//        boolean general_profile_space = reader.readBit();
//        int general_tier_flag = reader.readBit() ? 1 : 0;
//        int general_profile_idc = reader.readBits(5);
//        BitSet general_profile_compatibility_flag = new BitSet(32);
//        for (int i = 0; i < 32; i++) {
//            general_profile_compatibility_flag.set(i, reader.readBit());
//        }
//        int general_progressive_source_flag = reader.readBit() ? 1 : 0;
//        int general_interlaced_source_flag = reader.readBit() ? 1 : 0;
//        int general_non_packed_constraint_flag = reader.readBit() ? 1 : 0;
//        int general_frame_only_constraint_flag = reader.readBit() ? 1 : 0;
//        // 省略一些标志位的读取，可根据标准继续补充
//        int general_level_idc = reader.readBits(8);
//
//        Log.e(TAG,"general_profile_space: " + general_profile_space);
//        Log.e(TAG,"general_tier_flag: " + general_tier_flag);
//        Log.e(TAG,"general_profile_idc: " + general_profile_idc);
//        Log.e(TAG,"general_profile_compatibility_flag: " + general_profile_compatibility_flag);
//        Log.e(TAG,"general_progressive_source_flag: " + general_progressive_source_flag);
//        Log.e(TAG,"general_interlaced_source_flag: " + general_interlaced_source_flag);
//        Log.e(TAG,"general_non_packed_constraint_flag: " + general_non_packed_constraint_flag);
//        Log.e(TAG,"general_frame_only_constraint_flag: " + general_frame_only_constraint_flag);
//        Log.e(TAG,"general_level_idc: " + general_level_idc);
//
//        // 后续信息
//        int vps_sub_layer_ordering_info_present_flag = reader.readBit() ? 1 : 0;
//        for (int i = (vps_sub_layer_ordering_info_present_flag != 0)? 0 : vps_max_sub_layers_minus1; i <= vps_max_sub_layers_minus1; i++) {
//            int vps_max_dec_pic_buffering_minus1 = reader.readUEG();
//            int vps_max_num_reorder_pics = reader.readUEG();
//            int vps_max_latency_increase_plus1 = reader.readUEG();
//            Log.e(TAG,"For sub - layer " + i + ":");
//            Log.e(TAG,"  vps_max_dec_pic_buffering_minus1: " + vps_max_dec_pic_buffering_minus1);
//            Log.e(TAG,"  vps_max_num_reorder_pics: " + vps_max_num_reorder_pics);
//            Log.e(TAG,"  vps_max_latency_increase_plus1: " + vps_max_latency_increase_plus1);
//        }
//
//        int vps_max_layer_id = reader.readBits(6);
//        int vps_num_layer_sets_minus1 = reader.readUEG();
//        Log.e(TAG,"vps_max_layer_id: " + vps_max_layer_id);
//        Log.e(TAG,"vps_num_layer_sets_minus1: " + vps_num_layer_sets_minus1);
//
//        for (int i = 1; i <= vps_num_layer_sets_minus1; i++) {
//            for (int j = 0; j <= vps_max_layer_id; j++) {
//                int layer_id_included_flag = reader.readBit() ? 1 : 0;
//                Log.e(TAG,"For layer set " + i + ", layer " + j + " included flag: " + layer_id_included_flag);
//            }
//        }
//
//        int vps_timing_info_present_flag = reader.readBit() ? 1 : 0;
//        if (vps_timing_info_present_flag != 0) {
//            long vps_num_units_in_tick = reader.readBits(32);
//            long vps_time_scale = reader.readBits(32);
//            int vps_poc_proportional_to_timing_flag = reader.readBit() ? 1 : 0;
//            if (vps_poc_proportional_to_timing_flag != 0) {
//                int vps_num_ticks_poc_diff_one_minus1 = reader.readUEG();
//                Log.e(TAG,"vps_num_ticks_poc_diff_one_minus1: " + vps_num_ticks_poc_diff_one_minus1);
//            }
//            int vps_num_hrd_parameters = reader.readUEG();
//            Log.e(TAG,"vps_num_units_in_tick: " + vps_num_units_in_tick);
//            Log.e(TAG,"vps_time_scale: " + vps_time_scale);
//            Log.e(TAG,"vps_poc_proportional_to_timing_flag: " + vps_poc_proportional_to_timing_flag);
//            Log.e(TAG,"vps_num_hrd_parameters: " + vps_num_hrd_parameters);
//            for (int i = 0; i < vps_num_hrd_parameters; i++) {
//                int hrd_layer_set_idx = (i == 0)? 0 : reader.readUEG();
//                int cprms_present_flag = reader.readBit() ? 1 : 0;
//                // 省略 HRD 相关更详细的解析，可根据标准补充
//                Log.e(TAG,"For HRD parameter " + i + ":");
//                Log.e(TAG,"  hrd_layer_set_idx: " + hrd_layer_set_idx);
//                Log.e(TAG,"  cprms_present_flag: " + cprms_present_flag);
//            }
//        }
//
//        int vps_extension_flag = reader.readBit() ? 1 : 0;
//        if (vps_extension_flag != 0) {
//            // 处理 VPS 扩展部分，可根据具体扩展内容解析
//            Log.e(TAG,"VPS has extension data.");
//        }
    }

    // 解析 SPS
    public static void parseSPS(byte[] buffer) {
        BitstreamReader11 reader = new BitstreamReader11(buffer);

        // 0x42 0x01  -> 0100 0010 0000 0001
        // NAL 单元头
        reader.readBits(16);

        // SPS 基本信息
        int sps_video_parameter_set_id = reader.readBits(4);
        int sps_max_sub_layers_minus1 = reader.readBits(3);
        int sps_temporal_id_nesting_flag = reader.readBit() ? 1 : 0;
        Log.e(TAG,"SPS Basic Info:");
        Log.e(TAG,"  sps_video_parameter_set_id: " + sps_video_parameter_set_id);
        Log.e(TAG,"  sps_max_sub_layers_minus1: " + sps_max_sub_layers_minus1);
        Log.e(TAG,"  sps_temporal_id_nesting_flag: " + sps_temporal_id_nesting_flag);

        // 配置文件和层级信息
        boolean general_profile_space = reader.readBit();
        int general_tier_flag = reader.readBit() ? 1 : 0;
        int general_profile_idc = reader.readBits(5);
        int[] general_profile_compatibility_flag = new int[32];
        for (int i = 0; i < 32; i++) {
            general_profile_compatibility_flag[i] = reader.readBit() ? 1 : 0;
        }
        int general_progressive_source_flag = reader.readBit() ? 1 : 0;
        int general_interlaced_source_flag = reader.readBit() ? 1 : 0;
        int general_non_packed_constraint_flag = reader.readBit() ? 1 : 0;
        int general_frame_only_constraint_flag = reader.readBit() ? 1 : 0;
        int general_level_idc = reader.readBits(8);
        int profile_compatible_flag_sum = 0;
        for (int flag : general_profile_compatibility_flag) {
            profile_compatible_flag_sum += flag;
        }
        int sub_layer_profile_present_flag = (profile_compatible_flag_sum > 0) ? reader.readBit() ? 1 : 0 : 0;
        int sub_layer_level_present_flag = (profile_compatible_flag_sum > 0) ? reader.readBit() ? 1 : 0 : 0;
        Log.e(TAG,"Profile and Tier Info:");
        Log.e(TAG,"  general_profile_space: " + general_profile_space);
        Log.e(TAG,"  general_tier_flag: " + general_tier_flag);
        Log.e(TAG,"  general_profile_idc: " + general_profile_idc);
        Log.e(TAG,"  general_progressive_source_flag: " + general_progressive_source_flag);
        Log.e(TAG,"  general_interlaced_source_flag: " + general_interlaced_source_flag);
        Log.e(TAG,"  general_non_packed_constraint_flag: " + general_non_packed_constraint_flag);
        Log.e(TAG,"  general_frame_only_constraint_flag: " + general_frame_only_constraint_flag);
        Log.e(TAG,"  general_level_idc: " + general_level_idc);
        Log.e(TAG,"  sub_layer_profile_present_flag: " + sub_layer_profile_present_flag);
        Log.e(TAG,"  sub_layer_level_present_flag: " + sub_layer_level_present_flag);

        if (sub_layer_profile_present_flag == 1 || sub_layer_level_present_flag == 1) {
            for (int i = 0; i < sps_max_sub_layers_minus1; i++) {
                if (sub_layer_profile_present_flag == 1) {
                    boolean sub_layer_profile_space = reader.readBit();
                    int sub_layer_tier_flag = reader.readBit() ? 1 : 0;
                    int sub_layer_profile_idc = reader.readBits(5);
                    int[] sub_layer_profile_compatibility_flag = new int[32];
                    for (int j = 0; j < 32; j++) {
                        sub_layer_profile_compatibility_flag[j] = reader.readBit() ? 1 : 0;
                    }
                    int sub_layer_progressive_source_flag = reader.readBit() ? 1 : 0;
                    int sub_layer_interlaced_source_flag = reader.readBit() ? 1 : 0;
                    int sub_layer_non_packed_constraint_flag = reader.readBit() ? 1 : 0;
                    int sub_layer_frame_only_constraint_flag = reader.readBit() ? 1 : 0;
                    int sub_layer_level_idc = reader.readBits(8);
                    Log.e(TAG,"Sub - layer " + i + " Profile and Tier:");
                    Log.e(TAG,"    sub_layer_profile_space: " + sub_layer_profile_space);
                    Log.e(TAG,"    sub_layer_tier_flag: " + sub_layer_tier_flag);
                    Log.e(TAG,"    sub_layer_profile_idc: " + sub_layer_profile_idc);
                    Log.e(TAG,"    sub_layer_progressive_source_flag: " + sub_layer_progressive_source_flag);
                    Log.e(TAG,"    sub_layer_interlaced_source_flag: " + sub_layer_interlaced_source_flag);
                    Log.e(TAG,"    sub_layer_non_packed_constraint_flag: " + sub_layer_non_packed_constraint_flag);
                    Log.e(TAG,"    sub_layer_frame_only_constraint_flag: " + sub_layer_frame_only_constraint_flag);
                    Log.e(TAG,"    sub_layer_level_idc: " + sub_layer_level_idc);
                }
            }
        }

        // SPS 序列 ID
        int sps_seq_parameter_set_id = reader.readUEG();
        Log.e(TAG,"SPS Sequence ID:");
        Log.e(TAG,"  sps_seq_parameter_set_id: " + sps_seq_parameter_set_id);

        // 色度格式
        int chroma_format_idc = reader.readUEG();
        int separate_colour_plane_flag = 0;
        if (chroma_format_idc == 3) {
            separate_colour_plane_flag = reader.readBit() ? 1 : 0;
        }
        Log.e(TAG,"Chroma Format:");
        Log.e(TAG,"  chroma_format_idc: " + chroma_format_idc);
        Log.e(TAG,"  separate_colour_plane_flag: " + separate_colour_plane_flag);

        // 图像尺寸
        int pic_width_in_luma_samples = reader.readUEG();
        int pic_height_in_luma_samples = reader.readUEG();
        Log.e(TAG,"Picture Size:");
        Log.e(TAG,"  pic_width_in_luma_samples: " + pic_width_in_luma_samples);
        Log.e(TAG,"  pic_height_in_luma_samples: " + pic_height_in_luma_samples);

        // 合规窗口
        int conformance_window_flag = reader.readBit() ? 1 : 0;
        int conf_win_left_offset = 0;
        int conf_win_right_offset = 0;
        int conf_win_top_offset = 0;
        int conf_win_bottom_offset = 0;
        if (conformance_window_flag == 1) {
            conf_win_left_offset = reader.readUEG();
            conf_win_right_offset = reader.readUEG();
            conf_win_top_offset = reader.readUEG();
            conf_win_bottom_offset = reader.readUEG();
        }
        Log.e(TAG,"Conformance Window:");
        Log.e(TAG,"  conformance_window_flag: " + conformance_window_flag);
        Log.e(TAG,"  conf_win_left_offset: " + conf_win_left_offset);
        Log.e(TAG,"  conf_win_right_offset: " + conf_win_right_offset);
        Log.e(TAG,"  conf_win_top_offset: " + conf_win_top_offset);
        Log.e(TAG,"  conf_win_bottom_offset: " + conf_win_bottom_offset);

        // 位深度
        int bit_depth_luma_minus8 = reader.readUEG();
        int bit_depth_chroma_minus8 = reader.readUEG();
        Log.e(TAG,"Bit Depth:");
        Log.e(TAG,"  bit_depth_luma_minus8: " + bit_depth_luma_minus8);
        Log.e(TAG,"  bit_depth_chroma_minus8: " + bit_depth_chroma_minus8);

        // 最大 POC LSB 对数
        int log2_max_pic_order_cnt_lsb_minus4 = reader.readUEG();
        Log.e(TAG,"Max POC LSB Log2:");
        Log.e(TAG,"  log2_max_pic_order_cnt_lsb_minus4: " + log2_max_pic_order_cnt_lsb_minus4);

        // 时域子层排序信息
        int sps_sub_layer_ordering_info_present_flag = reader.readBit() ? 1 : 0;
        for (int i = (sps_sub_layer_ordering_info_present_flag != 0) ? 0 
                : sps_max_sub_layers_minus1; i <= sps_max_sub_layers_minus1; i++) {
            int sps_max_dec_pic_buffering_minus1 = reader.readUEG();
            int sps_max_num_reorder_pics = reader.readUEG();
            int sps_max_latency_increase_plus1 = reader.readUEG();
            Log.e(TAG,"Temporal Sub - layer " + i + " Ordering:");
            Log.e(TAG,"    sps_max_dec_pic_buffering_minus1: " + sps_max_dec_pic_buffering_minus1);
            Log.e(TAG,"    sps_max_num_reorder_pics: " + sps_max_num_reorder_pics);
            Log.e(TAG,"    sps_max_latency_increase_plus1: " + sps_max_latency_increase_plus1);
        }

        // 分块信息
        int log2_min_luma_coding_block_size_minus3 = reader.readUEG();
        int log2_diff_max_min_luma_coding_block_size = reader.readUEG();
        int log2_min_transform_block_size_minus2 = reader.readUEG();
        int log2_diff_max_min_transform_block_size = reader.readUEG();
        int max_transform_hierarchy_depth_inter = reader.readUEG();
        int max_transform_hierarchy_depth_intra = reader.readUEG();
        Log.e(TAG,"Partitioning Info:");
        Log.e(TAG,"  log2_min_luma_coding_block_size_minus3: " + log2_min_luma_coding_block_size_minus3);
        Log.e(TAG,"  log2_diff_max_min_luma_coding_block_size: " + log2_diff_max_min_luma_coding_block_size);
        Log.e(TAG,"  log2_min_transform_block_size_minus2: " + log2_min_transform_block_size_minus2);
        Log.e(TAG,"  log2_diff_max_min_transform_block_size: " + log2_diff_max_min_transform_block_size);
        Log.e(TAG,"  max_transform_hierarchy_depth_inter: " + max_transform_hierarchy_depth_inter);
        Log.e(TAG,"  max_transform_hierarchy_depth_intra: " + max_transform_hierarchy_depth_intra);

        // 缩放列表信息
        int scaling_list_enabled_flag = reader.readBit() ? 1 : 0;
        if (scaling_list_enabled_flag == 1) {
            int sps_scaling_list_data_present_flag = reader.readBit() ? 1 : 0;
            if (sps_scaling_list_data_present_flag == 1) {
                // 这里可以进一步解析缩放列表数据，目前仅输出标志
                Log.e(TAG,"Scaling List Data Present:");
                Log.e(TAG,"  sps_scaling_list_data_present_flag: " + sps_scaling_list_data_present_flag);
            }
        }
        Log.e(TAG,"Scaling List:");
        Log.e(TAG,"  scaling_list_enabled_flag: " + scaling_list_enabled_flag);

        // 量化参数信息
        int amp_enabled_flag = reader.readBit() ? 1 : 0;
        int sample_adaptive_offset_enabled_flag = reader.readBit() ? 1 : 0;
        int pcm_enabled_flag = reader.readBit() ? 1 : 0;
        if (pcm_enabled_flag == 1) {
            int pcm_sample_bit_depth_luma_minus1 = reader.readBits(4);
            int pcm_sample_bit_depth_chroma_minus1 = reader.readBits(4);
            int log2_min_pcm_luma_coding_block_size_minus3 = reader.readUEG();
            int log2_diff_max_min_pcm_luma_coding_block_size = reader.readUEG();
            int pcm_loop_filter_disabled_flag = reader.readBit() ? 1 : 0;
            Log.e(TAG,"PCM Info:");
            Log.e(TAG,"  pcm_sample_bit_depth_luma_minus1: " + pcm_sample_bit_depth_luma_minus1);
            Log.e(TAG,"  pcm_sample_bit_depth_chroma_minus1: " + pcm_sample_bit_depth_chroma_minus1);
            Log.e(TAG,"  log2_min_pcm_luma_coding_block_size_minus3: " + log2_min_pcm_luma_coding_block_size_minus3);
            Log.e(TAG,"  log2_diff_max_min_pcm_luma_coding_block_size: " + log2_diff_max_min_pcm_luma_coding_block_size);
            Log.e(TAG,"  pcm_loop_filter_disabled_flag: " + pcm_loop_filter_disabled_flag);
        }
        Log.e(TAG,"Quantization and PCM:");
        Log.e(TAG,"  amp_enabled_flag: " + amp_enabled_flag);
        Log.e(TAG,"  sample_adaptive_offset_enabled_flag: " + sample_adaptive_offset_enabled_flag);
        Log.e(TAG,"  pcm_enabled_flag: " + pcm_enabled_flag);

        // 其他信息
        int sps_loop_filter_across_slices_enabled_flag = reader.readBit() ? 1 : 0;
        int sps_cabac_init_present_flag = reader.readBit() ? 1 : 0;
        int log2_sao_offset_scale_luma = reader.readUEG();
        int log2_sao_offset_scale_chroma = reader.readUEG();
        Log.e(TAG,"Other Info:");
        Log.e(TAG,"  sps_loop_filter_across_slices_enabled_flag: " + sps_loop_filter_across_slices_enabled_flag);
        Log.e(TAG,"  sps_cabac_init_present_flag: " + sps_cabac_init_present_flag);
        Log.e(TAG,"  log2_sao_offset_scale_luma: " + log2_sao_offset_scale_luma);
        Log.e(TAG,"  log2_sao_offset_scale_chroma: " + log2_sao_offset_scale_chroma);
    }

    // 解析 PPS
    public static void parsePPS(byte[] buffer) {
        BitstreamReader11 reader = new BitstreamReader11(buffer);
        int pps_pic_parameter_set_id = reader.readUEG();
        int pps_seq_parameter_set_id = reader.readUEG();
        int dependent_slice_segments_enabled_flag = reader.readBit() ? 1 : 0;
        int output_flag_present_flag = reader.readBit() ? 1 : 0;
        int num_extra_slice_header_bits = reader.readBits(3);
        int sign_data_hiding_enabled_flag = reader.readBit() ? 1 : 0;
        int cabac_init_present_flag = reader.readBit() ? 1 : 0;

         Log.e(TAG, "PPS Attributes:");
         Log.e(TAG, "  pps_pic_parameter_set_id: " + pps_pic_parameter_set_id);
         Log.e(TAG, "  pps_seq_parameter_set_id: " + pps_seq_parameter_set_id);
         Log.e(TAG, "  dependent_slice_segments_enabled_flag: " + dependent_slice_segments_enabled_flag);
         Log.e(TAG, "  output_flag_present_flag: " + output_flag_present_flag);
         Log.e(TAG, "  num_extra_slice_header_bits: " + num_extra_slice_header_bits);
         Log.e(TAG, "  sign_data_hiding_enabled_flag: " + sign_data_hiding_enabled_flag);
         Log.e(TAG, "  cabac_init_present_flag: " + cabac_init_present_flag);
    }

    public static void main(String[] args) {
        String filePath = "path/to/your/h265_stream.h265";
        try (InputStream inputStream = new FileInputStream(filePath)) {
            byte[] buffer = new byte[1024 * 1024]; // 1MB 缓冲区
            int bytesRead = inputStream.read(buffer);
            int start = 0;
            while (start < bytesRead) {
                int nalUnitType = (buffer[start] & 0x7E) >> 1;
                int nextStart = start + 1;
                while (nextStart < bytesRead - 3 &&!(buffer[nextStart] == 0 && buffer[nextStart + 1] == 0 && (buffer[nextStart + 2] == 0 && buffer[nextStart + 3] == 1 || buffer[nextStart + 2] == 1))) {
                    nextStart++;
                }
                byte[] nalUnit = new byte[nextStart - start];
                System.arraycopy(buffer, start, nalUnit, 0, nalUnit.length);
                switch (nalUnitType) {
                    case 32: // VPS
                        parseVPS(nalUnit);
                        break;
                    case 33: // SPS
                        parseSPS(nalUnit);
                        break;
                    case 34: // PPS
                        parsePPS(nalUnit);
                        break;
                }
                start = nextStart;
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
