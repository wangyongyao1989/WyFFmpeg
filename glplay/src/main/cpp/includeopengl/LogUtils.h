//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2024/10/11.
//

#ifndef MYYFFMPEG_LOGUTILS_H
#define MYYFFMPEG_LOGUTILS_H

#include <android/log.h>

#define LOG_TAG "wy"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#endif //MYYFFMPEG_LOGUTILS_H
