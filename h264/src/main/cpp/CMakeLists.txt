cmake_minimum_required(VERSION 3.22.1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

project("h264lib")


set(distribution_DIR ${CMAKE_CURRENT_SOURCE_DIR})


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14")

set(jnilibs ${CMAKE_SOURCE_DIR}/jniLibs)

include_directories(
        h264include/h264nal
        h264include/webrtc
        h264include/x264

)

add_library(
        h264lib
        SHARED

        H264JniCall.cpp

        h264/bit_buffer.cc
        h264/h264_bitstream_parser.cc
        h264/h264_bitstream_parser_state.cc
        h264/h264_common.cc
        h264/h264_dec_ref_pic_marking_parser.cc
        h264/h264_hrd_parameters_parser.cc
        h264/h264_nal_unit_header_parser.cc
        h264/h264_nal_unit_header_svc_extension_parser.cc
        h264/h264_nal_unit_parser.cc
        h264/h264_nal_unit_payload_parser.cc
        h264/h264_pps_parser.cc
        h264/h264_pred_weight_table_parser.cc
        h264/h264_prefix_nal_unit_parser.cc
        h264/h264_ref_pic_list_modification_parser.cc
        h264/h264_rtp_fua_parser.cc
        h264/h264_rtp_parser.cc
        h264/h264_rtp_single_parser.cc
        h264/h264_rtp_stapa_parser.cc
        h264/h264_slice_header_in_scalable_extension_parser.cc
        h264/h264_slice_header_parser.cc
        h264/h264_slice_layer_extension_rbsp_parser.cc
        h264/h264_slice_layer_without_partitioning_rbsp_parser.cc
        h264/h264_sps_parser.cc
        h264/h264_sps_svc_extension_parser.cc
        h264/h264_subset_sps_parser.cc
        h264/h264_vui_parameters_parser.cc

        h264nal.cc
        H264NaluParse.cpp


)

link_directories(
        ${CMAKE_CURRENT_LIST_DIR}/libs/${ANDROID_ABI})

message(${CMAKE_CURRENT_LIST_DIR}/libs/${ANDROID_ABI}===ssss==)


target_include_directories(
        h264lib
        PRIVATE
        ${distribution_DIR}/h265include)


set(third-party-libs
                ${jnilibs}/${ANDROID_ABI}/libx264.a

)


target_link_libraries(
        h264lib
        # List libraries link to the target library
        android
        EGL
        GLESv3
        OpenSLES
        vulkan
        mediandk
        log
        ${third-party-libs}

)