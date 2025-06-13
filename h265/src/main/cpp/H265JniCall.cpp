
#include <jni.h>
#include <string>
#include <android/log.h>

#include "h265_bitstream_parser.h"
#include "h265_common.h"
#include "bit_buffer.h"

#include "HevcLogUtils.h"
#include "HevcNalParse.h"
#include "Hevc2MP4.h"

#include "media/NdkMediaCrypto.h"
#include "media/NdkMediaCodec.h"
#include "media/NdkMediaError.h"
#include "media/NdkMediaFormat.h"
#include "media/NdkMediaMuxer.h"
#include "media/NdkMediaExtractor.h"


//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2025/3/18.
//

using namespace std;
//包名+类名字符串定义：
const char *java_call_jni_class = "com/wangyongyao/h265/H265CallJni";
using namespace h265nal;


HevcNalParse *hevcNalParse;
Hevc2MP4 *hevc2MP4;


extern "C"
JNIEXPORT void JNICALL
cpp_test_h265(JNIEnv *env, jobject thiz, jstring dataPath) {
    const char *cDataPath = env->GetStringUTFChars(dataPath, nullptr);
    LOGE("cpp_test_h265=====%s", cDataPath);

    if (hevcNalParse == nullptr) {
        hevcNalParse = new HevcNalParse();
    }

    hevcNalParse->setHevcNalDataPath(cDataPath);

    env->ReleaseStringUTFChars(dataPath, cDataPath);

}

extern "C"
JNIEXPORT void JNICALL
cpp_hevc_to_mp4(JNIEnv *env, jobject thiz, jstring inputPath, jstring outputPath) {
//    const char *cInPath = env->GetStringUTFChars(inPath, nullptr);
//    const char *cOutPath = env->GetStringUTFChars(outPath, nullptr);
//
//    LOGE("cpp_hevc_to_mp4 cInPath:%s", cInPath);
//    LOGE("cpp_hevc_to_mp4 cOutPath:%s", cOutPath);
//    if (hevc2MP4 == nullptr) {
//        hevc2MP4 = new Hevc2MP4();
//    }
//
//    hevc2MP4->hevcConverterMp4(cInPath, cOutPath);
//
//
//    env->ReleaseStringUTFChars(inPath, cInPath);
//    env->ReleaseStringUTFChars(outPath, cOutPath);

    const char *input = env->GetStringUTFChars(inputPath, nullptr);
    const char *output = env->GetStringUTFChars(outputPath, nullptr);

    FILE *inputFile = fopen(input, "rb");
    if (!inputFile) {
        LOGE("Failed to open input file: %s", input);
        env->ReleaseStringUTFChars(inputPath, input);
        env->ReleaseStringUTFChars(outputPath, output);
        return ;
    }

    FILE *m_MediaMuxer_fp = fopen(output, "wb+");// 打开新建一个文件。
    if (m_MediaMuxer_fp == nullptr) {
        LOGE("MediaCodecMuxer:: outPath file fopen err!");
        return ;
    }
    // 由于muexr的原因，这里需要转换一下。
    size_t m_MediaMuxer_fd = fileno(m_MediaMuxer_fp);
    if (m_MediaMuxer_fd < 0) {
        LOGE("MediaCodecMuxer:: Mp4 file open err! = %d", m_MediaMuxer_fd);
        return ;
    }

    // 创建MediaMuxer
    AMediaMuxer *mediaMuxer = AMediaMuxer_new(m_MediaMuxer_fd, AMEDIAMUXER_OUTPUT_FORMAT_MPEG_4);
    if (!mediaMuxer) {
        LOGE("Failed to create MediaMuxer");
        fclose(inputFile);
        env->ReleaseStringUTFChars(inputPath, input);
        env->ReleaseStringUTFChars(outputPath, output);
        return ;
    }

    // 创建MediaFormat
    AMediaFormat *mediaFormat = AMediaFormat_new();

    // H.265 Advanced Video Coding
    AMediaFormat_setString(mediaFormat, AMEDIAFORMAT_KEY_MIME, "video/hevc");
    AMediaFormat_setInt32(mediaFormat, AMEDIAFORMAT_KEY_WIDTH, 1920);
    AMediaFormat_setInt32(mediaFormat, AMEDIAFORMAT_KEY_HEIGHT, 1080);
    AMediaFormat_setInt32(mediaFormat, AMEDIAFORMAT_KEY_COLOR_FORMAT,
                          COLOR_FormatYUV420Flexible);

    // 开始写入
    int videoTrackIndex = AMediaMuxer_addTrack(mediaMuxer, mediaFormat);
    if (videoTrackIndex < 0) {
        LOGE("Failed to add video track");
        AMediaFormat_delete(mediaFormat);
        AMediaMuxer_stop(mediaMuxer);
        AMediaMuxer_delete(mediaMuxer);
        fclose(inputFile);
        env->ReleaseStringUTFChars(inputPath, input);
        env->ReleaseStringUTFChars(outputPath, output);
        return ;
    }

    AMediaMuxer_start(mediaMuxer);

    const int bufferSize = 1 * 1024;
    uint8_t *buffer = new uint8_t[bufferSize];
    size_t bytesRead;
    int presentationTimeUs = 0;
    AMediaCodecBufferInfo *info;

    while ((bytesRead = fread(buffer, 1, bufferSize, inputFile)) > 0) {
        info = new AMediaCodecBufferInfo();
        info->offset = 0;
        info->size = bytesRead;
        info->presentationTimeUs = presentationTimeUs;
        info->flags = 0;
//        const uint8_t *data = buffer;
        LOGE("bytesRead====%d",bytesRead);

//        media_status_t status = AMediaMuxer_writeSampleData(mediaMuxer, videoTrackIndex, buffer,
//                                                            reinterpret_cast<const AMediaCodecBufferInfo *>(&info));
//        LOGE("status====%d",status);
//        if (status != AMEDIA_OK) {
//
//        }
        presentationTimeUs += (1000000 / 30); // 假设帧率为30fps
    }

    delete[] buffer;
    AMediaFormat_delete(mediaFormat);
    AMediaMuxer_stop(mediaMuxer);
    AMediaMuxer_delete(mediaMuxer);
    fclose(inputFile);

    env->ReleaseStringUTFChars(inputPath, input);
    env->ReleaseStringUTFChars(outputPath, output);

    LOGI("Conversion completed successfully");



}


static const JNINativeMethod methods[] = {
        {"n_test_h265",   "(Ljava/lang/String;)V", (void *) cpp_test_h265},
        {"n_hevc_to_mp4", "(Ljava/lang/String;"
                          "Ljava/lang/String;)V",  (void *) cpp_hevc_to_mp4},

};


// 定义注册方法
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    LOGD("动态注册");
    JNIEnv *env;
    if ((vm)->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        LOGD("动态注册GetEnv  fail");
        return JNI_ERR;
    }

    // 获取类引用
    jclass clazz = env->FindClass(java_call_jni_class);

    // 注册native方法
    jint regist_result = env->RegisterNatives(clazz, methods,
                                              sizeof(methods) / sizeof(methods[0]));
    if (regist_result) { // 非零true 进if
        LOGE("动态注册 fail regist_result = %d", regist_result);
    } else {
        LOGI("动态注册 success result = %d", regist_result);
    }
    return JNI_VERSION_1_6;
}

extern "C" void JNI_OnUnload(JavaVM *jvm, void *p) {
    JNIEnv *env = NULL;
    if (jvm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK) {
        return;
    }
    jclass clazz = env->FindClass(java_call_jni_class);
    if (env != NULL) {
        env->UnregisterNatives(clazz);
    }

}