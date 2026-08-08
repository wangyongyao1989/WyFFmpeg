# Android音视频开发实战：基于FFmpeg、OpenGL ES与RTMP的全栈解析

> 项目地址：[GitHub - wangyongyao1989/WyFFmpeg](https://github.com/wangyongyao1989/WyFFmpeg)
>
> 本文将深入剖析一个涵盖FFmpeg音视频处理、OpenGL ES渲染、H.264/H.265 NALU解析、RTMP直播推流的Android多媒体项目，从架构设计到代码实现逐层拆解。

---

## 目录

- [项目概述](#项目概述)
- [整体架构设计](#整体架构设计)
- [一、FFmpeg音视频处理模块（ffmpegplay）](#一ffmpeg音视频处理模块ffmpegplay)
  - [1.1 JNI动态注册机制](#11-jni动态注册机制)
  - [1.2 FFmpeg视频解码播放全流程](#12-ffmpeg视频解码播放全流程)
  - [1.3 FFmpeg音频解码播放](#13-ffmpeg音频解码播放)
  - [1.4 视频转码：MP4转AVI](#14-视频转码mp4转avi)
  - [1.5 视频水印添加](#15-视频水印添加)
- [二、OpenGL ES渲染模块（glplay）](#二opengl-es渲染模块glplay)
  - [2.1 双渲染模式架构](#21-双渲染模式架构)
  - [2.2 Camera预览通过OpenGL纹理渲染](#22-camera预览通过opengl纹理渲染)
  - [2.3 十三种滤镜效果实现](#23-十三种滤镜效果实现)
  - [2.4 FBO离屏渲染管线](#24-fbo离屏渲染管线)
  - [2.5 3D光照模型与手电筒效果](#25-3d光照模型与手电筒效果)
  - [2.6 FreeType文字渲染](#26-freetype文字渲染)
  - [2.7 视频录制：MediaCodec + EGL](#27-视频录制mediacodec--egl)
- [三、H.264 NALU解析模块（h264）](#三h264-nalu解析模块h264)
  - [3.1 解析架构与起始码查找](#31-解析架构与起始码查找)
  - [3.2 NAL单元类型与SPS解析](#32-nal单元类型与sps解析)
  - [3.3 帧率与分辨率计算](#33-帧率与分辨率计算)
- [四、H.265/HEVC解析与转MP4模块（h265）](#四h265hevc解析与转mp4模块h265)
  - [4.1 HEVC NAL头解析](#41-hevc-nal头解析)
  - [4.2 VPS/SPS/PPS提取（CSD-0）](#42-vpsspspps提取csd-0)
  - [4.3 HEVC转MP4封装实现](#43-hevc转mp4封装实现)
- [五、RTMP直播推流模块（rtmplive）](#五rtmp直播推流模块rtmplive)
  - [5.1 推流架构总览](#51-推流架构总览)
  - [5.2 x264视频编码与RTMP封装](#52-x264视频编码与rtmp封装)
  - [5.3 FAAC音频编码与RTMP封装](#53-faac音频编码与rtmp封装)
  - [5.4 RTMP连接与发送线程](#54-rtmp连接与发送线程)
  - [5.5 线程安全的生产者-消费者队列](#55-线程安全的生产者-消费者队列)
- [技术亮点与总结](#技术亮点与总结)

---

## 项目概述

WyFFmpeg是一个面向Android平台的多媒体全栈实践项目，通过JNI桥接Java与C++，整合了FFmpeg编解码、OpenGL ES 3.2渲染、H.264/H.265码流解析、RTMP实时推流等核心技术。项目包含7个Gradle模块，每个模块独立编译为动态库，职责清晰：

| 模块 | 核心功能 | 关键技术 |
|------|---------|---------|
| `ffmpegplay` | 音视频解码播放、转码、水印 | FFmpeg 6.x + ANativeWindow + OpenSLES |
| `glplay` | Camera预览、滤镜、3D光照、文字渲染、视频录制 | OpenGL ES 3.2 + GLSL + FreeType + MediaCodec |
| `h264` | H.264 NALU码流解析 | Facebook h264nal库 |
| `h265` | H.265 NALU解析 + HEVC转MP4 | Facebook h265nal库 + AMediaMuxer |
| `rtmplive` | RTMP直播推流 | librtmp + x264 + FAAC + Camera2 |
| `common` | 公共工具库 | - |
| `app` | 主应用入口 | - |

> GitHub仓库：[https://github.com/wangyongyao1989/WyFFmpeg](https://github.com/wangyongyao1989/WyFFmpeg)

---

## 整体架构设计

项目的`settings.gradle`定义了7个子模块：

```groovy
rootProject.name = "MyyFFmpeg"
include ':app'
include ':ffmpegplay'
include ':rtmplive'
include ':glplay'
include ':h265'
include ':h264'
include ':common'
```

各模块在C++层完全独立编译，不存在代码共享，通过各自的JNI接口暴露给Java层。统一的架构模式是：

```
Java View层 (GLSurfaceView / SurfaceView / TextureView)
        ↓  JNI动态注册 (RegisterNatives)
C++ JNI桥接层 (XxxJniCall.cpp)
        ↓  全局单例管理
C++ 功能管理器 (XxxManger / XxxRender)
        ↓  功能委派
C++ 核心实现 (FFmpeg / OpenGL / librtmp / x264 / FAAC)
```

所有模块均采用`armeabi-v7a` ABI，使用CMake 3.22.1构建，C++14标准。JNI统一使用**动态注册**模式，在`JNI_OnLoad`中通过`RegisterNatives`完成方法映射，避免了静态注册的性能开销和名称耦合。

---

## 一、FFmpeg音视频处理模块（ffmpegplay）

这是项目的核心模块之一，通过FFmpeg实现了视频解码播放、音频解码、视频转码和水印添加四大功能。C++层通过`FFmpegManger`外观类统一管理四大子模块。

### 1.1 JNI动态注册机制

JNI桥接层位于`FFPlayJniCalll.cpp`，采用动态注册方式。核心注册逻辑如下：

```cpp
// FFPlayJniCalll.cpp

// 全局单例
FFmpegManger *fmpegManger = nullptr;

static const JNINativeMethod methods[] = {
    {"stringFromJNI",  "()Ljava/lang/String;",       (void *)cpp_stringFromJNI},
    {"native_getFFmpegVersion", "()Ljava/lang/String;", (void *)cpp_getFFmpegVersion},
    {"native_MP4_AVI", "(Ljava/lang/String;Ljava/lang/String;)V", (void *)cpp_mp4_input_avi_output},
    {"native_Water_mark", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", (void *)cpp_mp4_water_mark},
    {"native_Play_Audio", "(Ljava/lang/String;)V",    (void *)cpp_play_audio},
    {"native_Play_init", "(Ljava/lang/String;Landroid/view/Surface;)V", (void *)cpp_play_init},
    {"native_Play_Video", "()V",                      (void *)cpp_play_video},
    {"native_Pause_Video", "()V",                     (void *)cpp_pause_video},
    {"native_Stop_Video", "()V",                     (void *)cpp_stop_video},
    {"native_seek_to_position", "(F)V",               (void *)cpp_seek_to_position},
    {"native_callback", "()V",                        (void *)cpp_init_callback},
    // ... 更多方法
};

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    jclass clazz = env->FindClass("com/example/ffmpegplay/FFPlayCallJni");
    env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0]));
    return JNI_VERSION_1_6;
}
```

Java侧的入口类`FFPlayCallJni.java`通过`System.loadLibrary("ffmpegplay")`加载native库，每个native方法都有对应的public包装方法供上层调用。同时声明了两个回调方法接收C++层的事件通知：

```java
// FFPlayCallJni.java
public class FFPlayCallJni {
    static {
        System.loadLibrary("ffmpegplay");
    }

    // native方法声明
    private native String native_getFFmpegVersion();
    private native void native_Play_init(String url, Surface surface);
    private native void native_Play_Video();
    private native void native_Pause_Video();
    private native void native_Stop_Video();
    private native void native_seek_to_position(float position);
    private native void native_callback();
    // ...

    // C++回调Java的方法
    public void CppEventCallback(int msgType, float msgValue) { ... }
    public void CppPlayStatusCallback(String status) { ... }
}
```

`FFmpegManger`作为外观模式（Facade）的核心，持有四大子模块的指针，并负责管理`JavaVM`和Java对象的全局引用，用于子线程回调：

```cpp
// FFmpegManger.h
class FFmpegManger {
private:
    FFmpegVideoTrans *fFmpegVideoTrans = nullptr;    // 转码
    FFmpegWaterMarkTest *waterMark = nullptr;         // 水印
    FFmpegPlayAudio *fmpegPlayAudio = nullptr;        // 音频
    FFmpegVideoPlay *videoPlay = nullptr;              // 视频
    JavaVM *mJavaVm = nullptr;
    jobject mJavaObj = nullptr;
    JNIEnv *mEnv = nullptr;
    // ...
};
```

### 1.2 FFmpeg视频解码播放全流程

`FFmpegVideoPlay`是项目中最完整的模块，实现了视频的解码、渲染、播放控制（播放/暂停/停止/Seek）全流程。

#### 关键数据结构

```cpp
// FFmpegVideoPlay.h
enum PlayerState {
    PLAYER_STATE_UNKNOWN,
    PLAYER_STATE_PLAYING,
    PLAYER_STATE_PAUSE,
    PLAYER_STATE_STOP
};

// 回调函数指针类型
typedef void (*MessageCallback)(void *, int, float);
typedef void (*PlaystatusCallback)(void *, const char *);

class FFmpegVideoPlay {
private:
    AVFormatContext *mAvFormatContext;   // 封装上下文
    AVCodecContext *mAvCodecContext;     // 编解码上下文
    ANativeWindow *mNativeWindow;       // 原生窗口（Surface渲染）
    SwsContext *mSwsContext;             // 图像格式转换器(YUV→RGBA)
    AVFrame *mAvFrame;                   // 解码后的原始帧
    AVFrame *mRgbFrame;                  // RGBA格式帧
    AVPacket *mAvPacket;                 // 压缩数据包
    std::thread *decodecThread;          // 解码线程
    std::mutex m_Mutex;                  // 互斥锁
    std::condition_variable m_Cond;      // 条件变量（暂停/恢复）
    std::atomic_bool pauseFlag;          // 暂停标志（原子操作）
    std::atomic_bool stopFlag;           // 停止标志
    volatile float m_SeekPosition;       // Seek位置
    // ...
};
```

#### 初始化流程

初始化分为三步：打开媒体文件、查找解码器、初始化原生窗口。

```cpp
// FFmpegVideoPlay.cpp
void FFmpegVideoPlay::init(JNIEnv *env, jobject thiz, const char *url, jobject surface) {
    initFFmeg();  // 核心初始化
}

void FFmpegVideoPlay::initFFmeg() {
    initFormatContext();    // 打开文件、获取流信息
    initFFmpegCodec();      // 查找视频流、打开解码器
    initANativeWindow();    // 初始化原生窗口、分配帧缓冲
}
```

**第一步：打开媒体文件**

```cpp
void FFmpegVideoPlay::initFormatContext() {
    mAvFormatContext = avformat_alloc_context();
    avformat_open_input(&mAvFormatContext, url, nullptr, nullptr);
    avformat_find_stream_info(mAvFormatContext, nullptr);
    av_dump_format(mAvFormatContext, 0, url, 0);
}
```

**第二步：查找视频流并打开解码器**

```cpp
void FFmpegVideoPlay::initFFmpegCodec() {
    // 遍历所有流，找到视频流
    for (int i = 0; i < mAvFormatContext->nb_streams; i++) {
        if (mAvFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            mVideoStreamIdx = i;
            break;
        }
    }

    // 获取编解码参数并查找解码器
    AVCodecParameters *codecParams = mAvFormatContext->streams[mVideoStreamIdx]->codecpar;
    const AVCodec *codec = avcodec_find_decoder(codecParams->codec_id);

    // 创建解码器上下文
    mAvCodecContext = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(mAvCodecContext, codecParams);

    // 设置字典选项（支持RTSP流）
    AVDictionary *dict = nullptr;
    av_dict_set(&dict, "buffer_size", "1024000", 0);
    av_dict_set(&dict, "stimeout", "20000000", 0);  // 超时20秒
    av_dict_set(&dict, "rtsp_transport", "tcp", 0);   // RTSP使用TCP传输

    avcodec_open2(mAvCodecContext, codec, &dict);
}
```

**第三步：初始化ANativeWindow和帧缓冲**

```cpp
void FFmpegVideoPlay::initANativeWindow() {
    // 从Java Surface获取原生窗口
    mNativeWindow = ANativeWindow_fromSurface(env, surface);

    // 分配帧缓冲
    mAvFrame = av_frame_alloc();
    mAvPacket = av_packet_alloc();
    mRgbFrame = av_frame_alloc();

    // 计算RGBA缓冲区大小
    int bufferSize = av_image_get_buffer_size(
        AV_PIX_FMT_RGBA, mAvCodecContext->width, mAvCodecContext->height, 1);
    uint8_t *outBuffer = (uint8_t *) av_malloc(bufferSize);

    // 绑定RGBA帧缓冲
    av_image_fill_arrays(mRgbFrame->data, mRgbFrame->linesize,
                         outBuffer, AV_PIX_FMT_RGBA,
                         mAvCodecContext->width, mAvCodecContext->height, 1);

    // 创建YUV→RGBA转换器（BICUBIC算法）
    mSwsContext = sws_getContext(
        mAvCodecContext->width, mAvCodecContext->height, mAvCodecContext->pix_fmt,
        mAvCodecContext->width, mAvCodecContext->height, AV_PIX_FMT_RGBA,
        SWS_BICUBIC, nullptr, nullptr, nullptr);

    // 设置窗口缓冲区格式
    ANativeWindow_setBuffersGeometry(mNativeWindow,
        mAvCodecContext->width, mAvCodecContext->height, WINDOW_FORMAT_RGBA_8888);
}
```

#### 解码主循环

解码在独立线程中执行，通过原子变量和条件变量实现暂停/停止/Seek控制：

```cpp
// FFmpegVideoPlay.cpp
void FFmpegVideoPlay::loopDecodec() {
    // 绑定RGBA帧缓冲
    av_image_fill_arrays(mRgbFrame->data, mRgbFrame->linesize,
                         mOutBuffer, AV_PIX_FMT_RGBA, videoWidth, videoHeight, 1);

    while (true) {
        // 暂停控制
        if (pauseFlag) {
            std::unique_lock<std::mutex> lock(m_Mutex);
            m_Cond.wait(lock);  // 阻塞等待恢复
        }

        // 停止控制
        if (stopFlag) {
            break;
        }

        // Seek控制
        if (m_SeekPosition > 0) {
            int64_t seekTarget = (int64_t)(m_SeekPosition * AV_TIME_BASE);
            avformat_seek_file(mAvFormatContext, -1, INT64_MIN,
                               seekTarget, INT64_MAX, AVSEEK_FLAG_BACKWARD);
            avcodec_flush_buffers(mAvCodecContext);
            m_SeekPosition = 0;
        }

        // 读取压缩包
        int ret = av_read_frame(mAvFormatContext, mAvPacket);
        if (ret != 0) break;

        // 仅处理视频流
        if (mAvPacket->stream_index == mVideoStreamIdx) {
            codecAvFrame();  // 解码
            sendFrameDataToANativeWindow();  // 渲染
        }
        av_packet_unref(mAvPacket);
    }
}
```

#### FFmpeg 3.x+分离式解码API

项目使用了FFmpeg的新版分离式解码API（`avcodec_send_packet` + `avcodec_receive_frame`），取代了旧版的`avcodec_decode_video2`：

```cpp
void FFmpegVideoPlay::codecAvFrame() {
    // 发送压缩包到解码器
    avcodec_send_packet(mAvCodecContext, mAvPacket);
    // 接收解码后的帧
    avcodec_receive_frame(mAvCodecContext, mAvFrame);
}
```

#### 渲染到屏幕

解码后的YUV数据通过`sws_scale`转换为RGBA，然后通过`ANativeWindow`渲染到屏幕：

```cpp
void FFmpegVideoPlay::sendFrameDataToANativeWindow() {
    // YUV → RGBA 格式转换
    sws_scale(mSwsContext,
              mAvFrame->data, mAvFrame->linesize, 0, mAvCodecContext->height,
              mRgbFrame->data, mRgbFrame->linesize);

    // 锁定窗口缓冲
    ANativeWindow_lock(mNativeWindow, &windowBuffer, nullptr);

    // 逐行拷贝RGBA数据（处理stride对齐）
    uint8_t *dst = (uint8_t *) windowBuffer.bits;
    int dstStride = windowBuffer.stride * 4;
    uint8_t *src = mRgbFrame->data[0];
    int srcStride = mRgbFrame->linesize[0];
    for (int h = 0; h < mAvCodecContext->height; h++) {
        memcpy(dst + h * dstStride, src + h * srcStride, srcStride);
    }

    // 根据帧类型回调状态
    if (mAvFrame->key_frame == 1) {
        mPlayStatusCallback(mMsgContext, "I");
    } else {
        mPlayStatusCallback(mMsgContext, "P/B");
    }

    // 帧率控制（约30fps）
    av_usleep(1000 * 33);

    // 解锁并提交显示
    ANativeWindow_unlockAndPost(mNativeWindow);
}
```

#### C++子线程回调Java

解码线程通过`FFmpegManger`的回调机制通知Java层播放状态：

```cpp
// FFmpegManger.cpp
void FFmpegManger::PostMessage(void *context, int msgType, float msgValue) {
    FFmpegManger *pManger = static_cast<FFmpegManger *>(context);
    bool isAttach = false;
    JNIEnv *env = pManger->GetJNIEnv(&isAttach);  // 子线程附加JNIEnv

    jclass clazz = env->GetObjectClass(pManger->mJavaObj);
    jmethodID mid = env->GetMethodID(clazz, "CppEventCallback", "(IF)V");
    env->CallVoidMethod(pManger->mJavaObj, mid, msgType, msgValue);

    if (isAttach) {
        pManger->mJavaVm->DetachCurrentThread();
    }
}
```

`GetJNIEnv`方法处理了子线程获取`JNIEnv`的关键问题：

```cpp
JNIEnv *FFmpegManger::GetJNIEnv(bool *isAttach) {
    JNIEnv *env;
    if (mJavaVm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        // 当前线程未 attached，需要手动挂载
        mJavaVm->AttachCurrentThread(&env, nullptr);
        *isAttach = true;
    }
    return env;
}
```

### 1.3 FFmpeg音频解码播放

`FFmpegPlayAudio`实现了音频的解码初始化和重采样配置。解码部分使用了FFmpeg API，重采样通过`SwrContext`将输出格式统一为`AV_SAMPLE_FMT_S16`（16位有符号整型，OpenSLES要求）：

```cpp
// FFmpegPlayAudio.cpp
void FFmpegPlayAudio::initFFmpeg(int *rate, int *channel, const char *inputUrl) {
    aFormatCtx = avformat_alloc_context();
    avformat_open_input(&aFormatCtx, inputUrl, nullptr, nullptr);
    avformat_find_stream_info(aFormatCtx, nullptr);

    // 查找音频流
    int audioStreamIdx = -1;
    for (int i = 0; i < aFormatCtx->nb_streams; i++) {
        if (aFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStreamIdx = i;
            break;
        }
    }

    // 查找并打开解码器
    const AVCodec *codec = avcodec_find_decoder(
        aFormatCtx->streams[audioStreamIdx]->codecpar->codec_id);
    aCodecCtx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(aCodecCtx, aFormatCtx->streams[audioStreamIdx]->codecpar);
    avcodec_open2(aCodecCtx, codec, nullptr);

    // 配置音频重采样器
    swr = swr_alloc();
    av_opt_set_channel_layout(swr, "in_channel_layout", aCodecCtx->channel_layout, 0);
    av_opt_set_channel_layout(swr, "out_channel_layout", aCodecCtx->channel_layout, 0);
    av_opt_set_int(swr, "in_sample_rate", aCodecCtx->sample_rate, 0);
    av_opt_set_int(swr, "out_sample_rate", aCodecCtx->sample_rate, 0);
    av_opt_set_sample_fmt(swr, "in_sample_fmt", aCodecCtx->sample_fmt, 0);
    av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);  // 输出S16
    swr_init(swr);

    // 分配PCM缓冲
    aFrame = av_frame_alloc();
    outputBufferSize = 8196;
    outputBuffer = (uint8_t *) malloc(outputBufferSize);

    *rate = aCodecCtx->sample_rate;
    *channel = aCodecCtx->channels;
}
```

音频数据流的设计意图是：文件 → `av_read_frame` → 解码 → `swr_convert`(S16) → PCM缓冲 → OpenSLES BufferQueue → 音频输出。

### 1.4 视频转码：MP4转AVI

`FFmpegVideoTrans`实现了MP4到AVI的封装格式转换，核心特点是**不重新解码/编码，仅转封装**，效率极高。关键技术点是H.264比特流格式转换——MP4使用mp4封装模式（无startcode，SPS/PPS在容器中），而AVI需要annexb模式（有startcode，SPS/PPS在ES中）：

```cpp
// FFmpegVideoTrans.cpp
void FFmpegVideoTrans::mp4ConversionAvi(const char *inPath, const char *outPath) {
    // 1. 打开输入MP4
    avformat_open_input(&inCtx, inPath, nullptr, nullptr);
    avformat_find_stream_info(inCtx, nullptr);

    // 2. 初始化H.264比特流过滤器（MP4→annexb）
    AVBitStreamFilterContext *vbsf = av_bitstream_filter_init("h264_mp4toannexb");

    // 3. 创建输出AVI上下文
    avformat_alloc_output_context2(&outCtx, nullptr, nullptr, outPath);

    // 4. 遍历流，创建输出流并复制编解码参数
    for (uint32_t i = 0; i < inCtx->nb_streams; i++) {
        AVStream *outStream = avformat_new_stream(outCtx, nullptr);
        avcodec_copy_context(outStream->codec, inCtx->streams[i]->codec);
        if (outCtx->oformat->flags & AVFMT_GLOBALHEADER) {
            outStream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }
    }

    // 5. 打开输出文件并写头
    avio_open(&outCtx->pb, outPath, AVIO_FLAG_WRITE);
    avformat_write_header(outCtx, nullptr);

    // 6. 转封装主循环
    AVPacket pkt;
    while (true) {
        if (av_read_frame(inCtx, &pkt) != 0) break;

        // 时间基转换
        pkt.pts = av_rescale_q_rnd(pkt.pts,
            inCtx->streams[pkt.stream_index]->time_base,
            outCtx->streams[pkt.stream_index]->time_base,
            AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
        pkt.dts = av_rescale_q_rnd(pkt.dts,
            inCtx->streams[pkt.stream_index]->time_base,
            outCtx->streams[pkt.stream_index]->time_base,
            AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
        pkt.duration = av_rescale_q(pkt.duration,
            inCtx->streams[pkt.stream_index]->time_base,
            outCtx->streams[pkt.stream_index]->time_base);

        // 视频流需要比特流格式转换
        if (pkt.stream_index == 0) {
            av_bitstream_filter_filter(vbsf,
                outCtx->streams[0]->codec, nullptr,
                &pkt.data, &pkt.size,
                pkt.data, pkt.size, pkt.flags & AV_PKT_FLAG_KEY);
        }

        av_write_frame(outCtx, &pkt);
        av_packet_unref(&pkt);
    }

    // 7. 写文件尾并释放资源
    av_write_trailer(outCtx);
    // ... cleanup
}
```

### 1.5 视频水印添加

`FFmpegWaterMarkTest`通过FFmpeg的filter graph实现水印叠加。滤镜描述使用`movie`滤镜加载水印图片，再通过`overlay`滤镜叠加到视频上：

```cpp
// FFmpegWaterMarkTest.cpp
void FFmpegWaterMarkTest::mp4WaterMark(const char *input,
                                        const char *png, const char *output) {
    const char *filter_descr = "movie=logo.jpg[wm];[in][wm]overlay=5:5[out]";

    // 1. 打开输入文件
    open_input_file(input);

    // 2. 初始化滤镜图
    init_filters(filter_descr);

    // 3. 解码→滤镜→输出循环
    while (av_read_frame(fmt_ctx, &pkt) >= 0) {
        if (pkt.stream_index == video_stream_index) {
            // 新版API解码
            avcodec_send_packet(dec_ctx, &pkt);
            avcodec_receive_frame(dec_ctx, frame);

            // 推入滤镜图
            av_buffersrc_add_frame_flags(buffersrc_ctx, frame, AV_BUFFERSRC_FLAG_KEEP_REF);

            // 拉取滤镜输出
            while (av_buffersink_get_frame(buffersink_ctx, filt_frame) >= 0) {
                // 写YUV文件
                // img2/{frame_number}.yuv
            }
        }
        av_packet_unref(&pkt);
    }
}
```

滤镜图的初始化过程遵循标准流程：创建buffer源过滤器和buffersink汇过滤器，解析滤镜描述字符串，配置整个graph：

```cpp
void FFmpegWaterMarkTest::init_filters(const char *filters_descr) {
    // 创建buffer源（输入端）
    const AVFilter *buffersrc = avfilter_get_by_name("buffer");
    buffersrc_ctx = avfilter_graph_alloc_filter(graph, buffersrc, "in");

    // 创建buffersink汇（输出端）
    const AVFilter *buffersink = avfilter_get_by_name("buffersink");
    buffersink_ctx = avfilter_graph_alloc_filter(graph, buffersink, "out");

    // 设置输出像素格式
    enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE };
    av_opt_set_int_list(buffersink_ctx, "pix_fmts", pix_fmts,
                        AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);

    // 创建输入输出端点
    AVFilterInOut *outputs = avfilter_inout_alloc();
    outputs->name = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx = 0;
    outputs->next = nullptr;

    AVFilterInOut *inputs = avfilter_inout_alloc();
    inputs->name = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx = 0;
    inputs->next = nullptr;

    // 解析滤镜描述并配置
    avfilter_graph_parse_ptr(graph, filters_descr, &inputs, &outputs, nullptr);
    avfilter_graph_config(graph, nullptr);
}
```

---

## 二、OpenGL ES渲染模块（glplay）

`glplay`是项目中最大的模块，基于OpenGL ES 3.2实现了Camera预览、13种滤镜、FBO后期处理、3D光照模型、FreeType文字渲染和视频录制。

### 2.1 双渲染模式架构

项目设计了两种渲染模式，根据场景需求选择：

**模式一：GLSurfaceView + Renderer回调**

适用于Camera预览、滤镜视频、3D光照等简单场景。GL上下文由`GLSurfaceView`自动管理：

```java
// Java侧
public class GLCameraPreView extends GLSurfaceView implements GLSurfaceView.Renderer {
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        // 传递着色器路径到C++
        OpenGLPlayCallJni.native_cameraPre_init(vertexPath, fragmentPath);
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        OpenGLPlayCallJni.native_cameraPre_setWH(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        OpenGLPlayCallJni.native_cameraPre_render(mMatrix);  // 传入变换矩阵
    }
}
```

**模式二：原生SurfaceView + EGL自管理**

适用于需要视频录制、离屏渲染等复杂场景。C++侧通过`EglCore` + `WindowSurface`自建EGL上下文，并通过`Looper`消息机制在独立线程处理GL命令：

```cpp
// EGLSurfaceViewVideoRender.cpp
void EGLSurfaceViewVideoRender::handleMessage(LooperMessage *msg) {
    switch (msg->what) {
        case MSG_SurfaceCreated:
            OnSurfaceCreated();   // 初始化EGL、着色器
            break;
        case MSG_SurfaceChanged:
            OnSurfaceChanged(msg->arg1, msg->arg2);  // 设置视口
            break;
        case MSG_DrawFrame:
            OnDrawFrame();         // 渲染一帧
            break;
        case MSG_SurfaceDestroyed:
            OnSurfaceDestroyed();  // 释放EGL
            break;
    }
}
```

JNI注册了约50个native方法，管理8个全局渲染器实例：

```cpp
// GLPlayJniCall.cpp
static const char *java_call_jni_class = "com/wangyongyao/glplay/OpenGLPlayCallJni";

// 全局渲染器实例
OpenglesFlashLight *flashLight;                    // 手电筒3D光照
OpenglesCameraPre *cameraPre;                      // Camera预览
OpenglesTexureVideoRender *textureVideoRender;     // Texture视频
OpenglesTextureFilterRender *filterRender;          // 滤镜视频
OpenglesSurfaceViewVideoRender *surfaceViewRender;  // SurfaceView视频
EGLSurfaceViewVideoRender *eglsurfaceViewRender;   // EGL视频+录制
GLDrawTextVideoRender *gLDrawTextVideoRender;      // 文字水印+录制
GLFBOPostProcessing *postProcessing;               // FBO后期处理
```

### 2.2 Camera预览通过OpenGL纹理渲染

Camera预览使用`GLSurfaceView` + `SurfaceTexture` + OES纹理的方式。Java侧通过Camera2获取预览帧，生成OES纹理ID后传递给C++层：

```cpp
// OpenglesCameraPre.cpp
// 顶点数据 - 4个顶点矩形（TRIANGLE_STRIP）
const float CAMERA_PRE_VERTEX[] = {
    0.5f, -0.5f,
    0.5f,  0.5f,
   -0.5f, -0.5f,
   -0.5f,  0.5f
};

// 纹理坐标（注意只取左上1/4区域，适配Camera预览的特殊纹理排布）
const float CAMERA_PRE_TEXTURE[] = {
    0.5f, 0.5f,
    0.5f, 0.0f,
    0.0f, 0.5f,
    0.0f, 0.0f
};

void OpenglesCameraPre::initGraphics() {
    // 创建GL程序
    mProgram = OpenGLShader::createProgram(vertexShader, fragmentShader);
    // 设置顶点属性指针
    // ...
    // 生成纹理ID返回给Java
    glGenTextures(1, &mTextureId);
}

void OpenglesCameraPre::renderFrame(float mtx[]) {
    glUseProgram(mProgram);
    // 绑定OES纹理
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, mTextureId);
    // 设置变换矩阵
    glUniformMatrix4fv(vMatrixLoc, 1, GL_FALSE, mtx);
    // 绘制
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
```

Camera预览的顶点着色器通过矩阵变换纹理坐标，实现相机预览的旋转/镜像适配：

```glsl
// camera_pre_vertex.glsl
#version 320 es
in vec3 aPos;
in vec4 aTexCoord;
out vec2 TexCoord;
uniform mat4 vMatrix;

void main() {
    gl_Position = vec4(aPos, 1.0);
    TexCoord = (vMatrix * aTexCoord).xy;  // 矩阵变换纹理坐标
}
```

### 2.3 十三种滤镜效果实现

滤镜系统由`OpenglesTextureFilterRender`管理，支持13种滤镜的动态切换。所有滤镜共享同一个顶点着色器，仅替换片段着色器：

```cpp
// OpenglesTextureFilterRender.cpp
void OpenglesTextureFilterRender::render() {
    // 滤镜切换检测
    if (m_filter != m_prevFilter) {
        m_prevFilter = m_filter;
        if (m_filter >= 0 && m_filter < m_fragmentStringPathes.size()) {
            delete_program(m_program);  // 删除旧程序
            // 加载新滤镜的着色器路径
            setSharderStringPath(m_vertexStringPath, m_fragmentStringPathes.at(m_filter));
            createProgram();  // 编译链接新程序
        }
    }
    OpenglesTexureVideoRender::render();  // 调用父类渲染
}
```

所有滤镜共用YUV→RGB转换公式（BT.601标准）：

```glsl
// YUV→RGB转换（所有滤镜共用）
vec4 YuvToRgb(vec2 uv) {
    float y = texture(s_textureY, uv).r;
    float u = texture(s_textureU, uv).r;
    float v = texture(s_textureV, uv).r;
    u = u - 0.5;
    v = v - 0.5;
    float r = y + 1.403 * v;
    float g = y - 0.344 * u - 0.714 * v;
    float b = y + 1.770 * u;
    return vec4(r, g, b, 1.0);
}
```

以下是几种代表性滤镜的GLSL实现：

**模糊滤镜**（4邻域采样平均）：

```glsl
// texture_filter1_play_frament.glsl
float step = 0.5 / 100.0f;
sample0 = YuvToRgb(vec2(v_texcoord.x - step, v_texcoord.y - step));
sample1 = YuvToRgb(vec2(v_texcoord.x + step, v_texcoord.y + step));
sample2 = YuvToRgb(vec2(v_texcoord.x + step, v_texcoord.y - step));
sample3 = YuvToRgb(vec2(v_texcoord.x - step, v_texcoord.y + step));
FragColor = (sample0 + sample1 + sample2 + sample3) / 4.0;
```

**鱼眼滤镜**（球面坐标映射）：

```glsl
// texture_filter2_play_frament.glsl
float aperture = 158.0;
float apertureHalf = 0.5 * aperture * (PI / 180.0);
float maxFactor = sin(apertureHalf);
vec2 xy = 2.0 * v_texcoord.xy - 1.0;
float d = length(xy);
if (d < (2.0 - maxFactor)) {
    d = length(xy * maxFactor);
    float z = sqrt(1.0 - d * d);
    float r = atan(d, z) / PI;
    float phi = atan(xy.y, xy.x);
    uv.x = r * cos(phi) + 0.5;
    uv.y = r * sin(phi) + 0.5;
}
```

**旋流滤镜**（以中心为原点的旋转扭曲）：

```glsl
// texture_filter3_play_frament.glsl
float radius = 200.0;
float angle = 0.8;
vec2 center = vec2(texSize.x / 2.0, texSize.y / 2.0);
vec2 tc = v_texcoord * texSize - center;
float dist = length(tc);
if (dist < radius) {
    float percent = (radius - dist) / radius;
    float theta = percent * percent * angle * 8.0;
    float s = sin(theta);
    float c = cos(theta);
    // 旋转矩阵变换
    tc = vec2(dot(tc, vec2(c, -s)), dot(tc, vec2(s, c)));
}
tc += center;
```

**卡通化滤镜**（色彩量化 + Sobel边缘检测）：

```glsl
// texture_filter9_play_frament.glsl
// 1. RGB→HSV色彩空间转换并量化
vec3 vHSV = RGBtoHSV(color.r, color.g, color.b);
vHSV.x = nearestLevel(vHSV.x, 0);  // 色调量化为6级
vHSV.y = nearestLevel(vHSV.y, 1);  // 饱和度量化为7级
vHSV.z = nearestLevel(vHSV.z, 2);  // 明度量化为4级

// 2. Sobel边缘检测
float edg = IsEdge(uv);

// 3. 边缘处输出黑色，非边缘处输出量化后的颜色
vec3 vRGB = (edg >= edge_thres)
    ? vec3(0.0)
    : HSVtoRGB(vHSV.x, vHSV.y, vHSV.z);
```

**边缘检测滤镜**（拉普拉斯卷积核）：

```glsl
// texture_filter12_play_frament.glsl
mat3 edgeDetectionKernel = mat3(
    -1, -1, -1,
    -1,  8, -1,
    -1, -1, -1
);
for(int i = 0; i < 3; i++) {
    for(int j = 0; j < 3; j++) {
        vec4 sampleColor = YuvToRgb(samplePos);
        color += sampleColor * edgeDetectionKernel[i][j];
    }
}
```

### 2.4 FBO离屏渲染管线

`GLFBOPostProcessing`实现了完整的FBO后期处理管线，包含三套独立的着色器程序：YUV视频渲染到FBO、图片水印叠加到FBO、FBO纹理经后期处理后输出到屏幕。

FBO创建流程：

```cpp
// GLFBOPostProcessing.cpp
void GLFBOPostProcessing::creatFBOTexture() {
    // 1. 创建帧缓冲对象
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // 2. 创建颜色附件纹理
    glGenTextures(1, &fboTexture);
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenW, screenH,
                 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, fboTexture, 0);

    // 3. 创建渲染缓冲对象（深度+模板）
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenW, screenH);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, rbo);

    // 4. 检查完整性
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
}
```

渲染流程：

```cpp
void GLFBOPostProcessing::renderFrame() {
    // 步骤1: 绑定FBO，启用深度测试
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glEnable(GL_DEPTH_TEST);

    // 步骤2: 绘制YUV视频纹理到FBO
    yuvGLShader->use();
    // 设置view/projection/model矩阵
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // 步骤3: 绘制图片水印到FBO
    bindPicTexture();
    usePicProgram();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // 步骤4: 解绑FBO，禁用深度测试
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);

    // 步骤5: 后期处理：FBO纹理 → 屏幕
    screenShader->use();
    useFBOProgram();
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // 步骤6: 交换缓冲区显示
    swapBuffers();
}
```

支持5种后期效果，通过运行时切换着色器程序实现：

```cpp
void GLFBOPostProcessing::renderFrame() {
    if (m_filter != m_prevFilter) {
        m_prevFilter = m_filter;
        delete_program(screenProgram);
        screenShader->getSharderStringPath(
            m_vertexStringPath,
            m_fragmentStringPathes.at(m_prevFilter));
        createFBOProgram();  // 重新编译链接新的后处理着色器
    }
    // ... 渲染
}
```

5种后期效果的核心算法：

| 效果 | 核心算法 |
|------|---------|
| 原图 | `texture(screenTexture, TexCoords).rgb` |
| 反相 | `1.0 - texture(screenTexture, TexCoords)` |
| 灰度（均值） | `(r + g + b) / 3.0` |
| 灰度（加权） | `0.2126*r + 0.7152*g + 0.0722*b` |
| 核效果（水平模糊） | 5-tap高斯权重 `[0.227, 0.195, 0.122, 0.054, 0.016]` |

### 2.5 3D光照模型与手电筒效果

`OpenglesFlashLight`实现了完整的Phong光照模型，包括环境光、漫反射、高光、距离衰减和聚光锥体。项目还实现了FPS风格的3D摄像机系统。

**3D摄像机系统**基于欧拉角实现：

```cpp
// OpenGLCamera3D.cpp
class OpenGLCamera3D {
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    float Yaw;    // 偏航角
    float Pitch;  // 俯仰角
    float Zoom;   // FOV

    glm::mat4 GetViewMatrix() {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void ProcessXYMovement(float xoffset, float yoffset) {
        Yaw   += xoffset;
        Pitch += yoffset;
        updateCameraVectors();
    }

    void ProcessScroll(float yoffset) {
        Zoom -= yoffset;
        if (Zoom < 25.0f) Zoom = 25.0f;
        if (Zoom > 100.0f) Zoom = 100.0f;
    }

    void updateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};
```

**手电筒渲染**：光源位置和方向跟随摄像机：

```cpp
// OpenglesFlashLight.cpp
void OpenglesFlashLight::renderFrame() {
    // 光源位置和方向 = 摄像机位置和朝向
    lightColorShader->setVec3("light.position", mCamera.Position);
    lightColorShader->setVec3("light.direction", mCamera.Front);
    lightColorShader->setFloat("light.cutOff",
        glm::cos(glm::radians(12.5f)));  // 内切光角

    // 光照属性
    lightColorShader->setVec3("light.ambient",  0.5f, 0.5f, 0.5f);
    lightColorShader->setVec3("light.diffuse",  1.0f, 1.0f, 1.0f);
    lightColorShader->setVec3("light.specular", 0.5f, 0.5f, 0.5f);

    // 距离衰减系数
    lightColorShader->setFloat("light.constant",  1.0f);
    lightColorShader->setFloat("light.linear",    0.09f);
    lightColorShader->setFloat("light.quadratic",  0.032f);

    // 材质属性
    lightColorShader->setFloat("material.shininess", 64.0f);

    // 绘制10个立方体
    for (unsigned int i = 0; i < 10; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, FlashLightCubePositions[i]);
        float angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle),
                           glm::vec3(1.0f, 0.3f, 0.5f));
        lightColorShader->setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}
```

**聚光手电筒片段着色器**实现了完整的Phong光照公式：

```glsl
// flash_light_color_fragment.glsl
#version 320 es
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 direction;
    float cutOff;       // 内切光角（余弦值）
    float outerCutOff;  // 外切光角
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

void main() {
    vec3 lightDir = normalize(light.position - FragPos);
    float theta = dot(lightDir, normalize(-light.direction));

    if (theta > light.cutOff) {  // 在聚光锥体内
        // 环境光
        vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

        // 漫反射
        vec3 norm = normalize(Normal);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

        // Phong高光
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

        // 距离衰减
        float distance = length(light.position - FragPos);
        float attenuation = 1.0 / (light.constant
                                   + light.linear * distance
                                   + light.quadratic * (distance * distance));
        diffuse *= attenuation;
        specular *= attenuation;

        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0);
    } else {
        // 锥体外仅环境光
        FragColor = vec4(light.ambient * texture(material.diffuse, TexCoords).rgb, 1.0);
    }
}
```

光照公式总结：
- 环境光：`ambient = light.ambient * diffuseMap`
- 漫反射：`diff = max(dot(N, L), 0)` → `diffuse = light.diffuse * diff * diffuseMap`
- Phong高光：`spec = pow(max(dot(V, R), 0), shininess)` → `specular = light.specular * spec * specularMap`
- 衰减：`attenuation = 1 / (c + l*d + q*d²)`
- 聚光锥体检测：`theta = dot(L, -direction)` 与 `cutOff` 比较

### 2.6 FreeType文字渲染

`GLDrawTextVideoRender`通过FreeType库实现了文字渲染，用于视频水印叠加。文字渲染使用正交投影 + Alpha混合。

**FreeType字体加载**：预加载ASCII前128个字符为纹理：

```cpp
// GLDrawTextVideoRender.cpp
void GLDrawTextVideoRender::LoadFacesByASCII(const char *path) {
    FT_Library ft;
    FT_Init_FreeType(&ft);
    FT_Face face;
    FT_New_Face(ft, path, 0, &face);     // 加载arial.ttf
    FT_Set_Pixel_Sizes(face, 0, 48);     // 48像素高度
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // 禁用字节对齐限制

    for (GLubyte c = 0; c < 128; c++) {
        FT_Load_Char(face, c, FT_LOAD_RENDER);  // 渲染字形到位图

        // 生成纹理
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
                     face->glyph->bitmap.width, face->glyph->bitmap.rows,
                     0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                     face->glyph->bitmap.buffer);

        // 存储字符信息
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<GLuint>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}
```

**文字渲染**：逐字符计算位置和大小，动态更新VBO：

```cpp
void GLDrawTextVideoRender::RenderText(std::string text, GLfloat x, GLfloat y,
                                       GLfloat scale, glm::vec3 color,
                                       glm::vec2 viewport) {
    textGLShader->use();
    textGLShader->setVec3("textColor", color.x, color.y, color.z);
    glBindVertexArray(VAO);

    for (auto c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];

        // 计算字符位置和大小
        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;

        // 每个字符的4个顶点（位置 + 纹理坐标）
        GLfloat vertices[6][4] = {
            {xpos,     ypos + h, 0.0, 0.0},
            {xpos,     ypos,     0.0, 1.0},
            {xpos + w, ypos,     1.0, 1.0},
            {xpos,     ypos + h, 0.0, 0.0},
            {xpos + w, ypos,     1.0, 1.0},
            {xpos + w, ypos + h, 1.0, 0.0}
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // 前进到下一个字符（advance单位是1/64像素）
        x += (ch.Advance >> 6) * scale;
    }
}
```

**文字着色器**使用字形红色通道作为alpha值：

```glsl
// gl_draw_text_fragment.glsl
uniform sampler2D text;
uniform vec3 textColor;

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    FragColor = vec4(textColor, 1.0) * sampled;
}
```

### 2.7 视频录制：MediaCodec + EGL

视频录制通过`MediaCodec`硬件编码器 + EGL共享上下文实现。核心思路是用`glBlitFramebuffer`将渲染结果从显示Surface复制到编码器输入Surface：

```cpp
// EGLSurfaceViewVideoRender.cpp
void EGLSurfaceViewVideoRender::startEncoder(const char *recordPath) {
    // 创建MediaCodec编码器
    m_VideoEncoderCore = new VideoEncoderCore(
        VIDEO_WIDTH, VIDEO_HEIGHT, BIT_RATE, recordPath);

    // 创建编码器输入Surface的EGL WindowSurface
    m_InputWindowSurface = new WindowSurface(
        m_EglCore, m_VideoEncoderCore->getInputSurface());

    // 启动编码线程
    m_TextureMovieEncoder2 = new TextureMovieEncoder2(m_VideoEncoderCore);
}

void EGLSurfaceViewVideoRender::OnDrawFrame() {
    // ... 正常渲染（YUV视频 + 图片水印 + 文字水印）...
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // 录制：将帧从显示Surface复制到编码器Surface
    if (m_InputWindowSurface != nullptr) {
        // 设置编码器Surface为当前读取源
        m_InputWindowSurface->makeCurrentReadFrom(*m_WindowSurface);
        glBlitFramebuffer(0, 0, m_backingWidth, m_backingHeight,
                         offX, offY, off_right, off_bottom,
                         GL_COLOR_BUFFER_BIT, GL_NEAREST);
        m_InputWindowSurface->swapBuffers();  // 提交给MediaCodec
    }

    // 恢复显示Surface并交换缓冲
    m_WindowSurface->makeCurrent();
    m_WindowSurface->swapBuffers();
}
```

`EglCore`改编自Google Grafika项目，支持`FLAG_RECORDABLE`标志使EGL Surface可被MediaCodec录制：

```cpp
// EglCore.cpp
EGLSurface EglCore::createWindowSurface(ANativeWindow *surface) {
    EGLint surfaceAttribs[] = {
        EGL_GL_COLORSPACE, EGL_GL_COLORSPACE_LINEAR,
        EGL_NONE
    };
    return eglCreateWindowSurface(mEGLDisplay, mEGLConfig, surface, surfaceAttribs);
}

void EglCore::setPresentationTime(EGLSurface surface, long nsecs) {
    eglPresentationTimeANDROID(mEGLDisplay, surface, nsecs);
}
```

---

## 三、H.264 NALU解析模块（h264）

`h264`模块使用Facebook开源的`h264nal`解析库，实现了H.264码流的完整解析，包括NALU起始码查找、NAL头解析、SPS/PPS提取、分辨率和帧率计算。

### 3.1 解析架构与起始码查找

解析库的核心入口是`H264BitstreamParser::ParseBitstream`，内部先查找所有NALU的起始码位置，再逐一解析：

```cpp
// H264NaluParse.cpp
void H264NaluParse::setHevcNalDataPath(const char *dataPath) {
    // 1. 读取整个H.264文件
    FILE *infp = fopen(dataPath, "rb");
    fseek(infp, 0, SEEK_END);
    int64_t size = ftell(infp);
    fseek(infp, 0, SEEK_SET);
    std::vector<uint8_t> buffer(size);
    fread(reinterpret_cast<char *>(buffer.data()), 1, size, infp);

    // 2. 调用h264nal解析整个比特流
    std::unique_ptr<h264nal::H264BitstreamParser::BitstreamState> bitstream =
        h264nal::H264BitstreamParser::ParseBitstream(
            buffer.data(), buffer.size(), bitstream_parser_state, false);
}
```

起始码查找使用类Boyer-Moore算法，查找`00 00 01`（3字节）或`00 00 00 01`（4字节）起始码：

```cpp
// h264_bitstream_parser.cc
void H264BitstreamParser::FindNaluIndices(const uint8_t *data, size_t length,
                                          std::vector<NaluIndex> *sequences) {
    for (size_t i = 0; i < end;) {
        if (data[i + 2] > 1) {
            i += 3;  // 快速跳过
        } else if (data[i + 2] == 0x01 && data[i + 1] == 0x00 && data[i] == 0x00) {
            NaluIndex index = {i, i + 3, 0};
            // 检测4字节起始码
            if (index.start_offset > 0 && data[index.start_offset - 1] == 0)
                --index.start_offset;
            sequences->push_back(index);
            i += 3;
        } else {
            ++i;
        }
    }
}
```

### 3.2 NAL单元类型与SPS解析

H.264的NAL头为1字节，包含`forbidden_zero_bit`(1bit) + `nal_ref_idc`(2bit) + `nal_unit_type`(5bit)：

```cpp
// h264_nal_unit_header_parser.cc
H264NalUnitHeaderParser::NalUnitHeader
H264NalUnitHeaderParser::ParseNalUnitHeader(BitBuffer *bit_buffer) {
    NalUnitHeader header;
    bit_buffer->ReadBits(&header.forbidden_zero_bit, 1);  // 禁止位
    bit_buffer->ReadBits(&header.nal_ref_idc, 2);          // NAL参考标识
    bit_buffer->ReadBits(&header.nal_unit_type, 5);         // NAL类型
    return header;
}
```

H.264 NAL单元类型定义：

```cpp
// h264_common.h
enum NalUnitType : uint8_t {
    UNSPECIFIED_NUT = 0,
    CODED_SLICE_OF_NON_IDR_PICTURE_NUT = 1,  // P/B帧（非IDR）
    CODED_SLICE_OF_IDR_PICTURE_NUT = 5,       // I帧（IDR关键帧）
    SEI_NUT = 6,                               // 补充增强信息
    SPS_NUT = 7,                               // 序列参数集
    PPS_NUT = 8,                               // 图像参数集
    AUD_NUT = 9,                               // 访问单元分隔符
    // ...
};
```

根据NAL类型分发到对应的payload解析器：

```cpp
// h264_nal_unit_payload_parser.cc
switch (nal_unit_header.nal_unit_type) {
    case SPS_NUT:
        nal_unit_payload->sps = H264SpsParser::ParseSps(bit_buffer);
        bitstream_parser_state->sps[sps_id] = nal_unit_payload->sps;
        break;
    case PPS_NUT:
        nal_unit_payload->pps = H264PpsParser::ParsePps(bit_buffer, ...);
        bitstream_parser_state->pps[pps_id] = nal_unit_payload->pps;
        break;
    case CODED_SLICE_OF_IDR_PICTURE_NUT:
    case CODED_SLICE_OF_NON_IDR_PICTURE_NUT:
        // 调用 SliceLayerWithoutPartitioningRbspParser
        break;
}
```

SPS解析按H.264标准7.3.2.1.1逐字段读取：

```cpp
// h264_sps_parser.cc
SpsData SpsDataParser::ParseSpsData(BitBuffer *bit_buffer) {
    SpsData sps;
    bit_buffer->ReadBits(&sps.profile_idc, 8);           // 档次
    // constraint_set0~5_flag
    bit_buffer->ReadBits(&sps.level_idc, 8);             // 等级
    sps.seq_parameter_set_id = ReadUE(bit_buffer);         // SPS ID
    sps.chroma_format_idc = ReadUE(bit_buffer);            // 色度格式
    sps.pic_width_in_mbs_minus1 = ReadUE(bit_buffer);      // 宽度（宏块数-1）
    sps.pic_height_in_map_units_minus1 = ReadUE(bit_buffer); // 高度
    bit_buffer->ReadBits(&sps.frame_mbs_only_flag, 1);    // 是否只有帧编码
    // 若vui_parameters_present_flag，解析VUI（含time_scale等）
    return sps;
}
```

### 3.3 帧率与分辨率计算

从SPS中提取分辨率和帧率：

```cpp
// H264NaluParse.cpp
// 分辨率计算（H.264需要乘以16）
uint32_t pic_width_in_mbs_minus1 =
    bitstream_parser_state->GetSps(0)->sps_data->pic_width_in_mbs_minus1;
uint32_t pic_height_in_map_units_minus1 =
    bitstream_parser_state->GetSps(0)->sps_data->pic_height_in_map_units_minus1;
uint32_t frame_mbs_only_flag =
    bitstream_parser_state->GetSps(0)->sps_data->frame_mbs_only_flag;

int width  = (pic_width_in_mbs_minus1 + 1) * 16;
int height = (pic_height_in_map_units_minus1 + 1) * 16 * (2 - frame_mbs_only_flag);

// 帧率计算（从VUI参数）
uint32_t timeScale =
    bitstream_parser_state->GetSps(0)->sps_data->vui_parameters->time_scale;
uint32_t numUnitsInTick =
    bitstream_parser_state->GetSps(0)->sps_data->vui_parameters->num_units_in_tick;

float frameRate = timeScale / (2.0 * numUnitsInTick);  // H.264帧率公式
```

注意H.264的分辨率需要用宏块数乘以16（每个宏块16x16像素），帧率公式分母有2.0的系数。

---

## 四、H.265/HEVC解析与转MP4模块（h265）

`h265`模块使用Facebook开源的`h265nal`解析库，在H.264的基础上增加了VPS（视频参数集）解析，并实现了HEVC裸流转MP4容器封装。

### 4.1 HEVC NAL头解析

H.265的NAL头为**2字节**（H.264为1字节），结构不同：

```cpp
// h265_nal_unit_header_parser.cc
H265NalUnitHeaderParser::NalUnitHeader
H265NalUnitHeaderParser::ParseNalUnitHeader(BitBuffer *bit_buffer) {
    NalUnitHeader header;
    bit_buffer->ReadBits(&header.forbidden_zero_bit, 1);   // 禁止位
    bit_buffer->ReadBits(&header.nal_unit_type, 6);          // NAL类型（6位）
    bit_buffer->ReadBits(&header.nuh_layer_id, 6);           // 层ID
    bit_buffer->ReadBits(&header.nuh_temporal_id_plus1, 3);  // 时间ID
    return header;
}
```

获取NAL类型的辅助方法：

```cpp
// HevcNalParse.cpp
int HevcNalParse::getNalUnitType(const std::vector<uint8_t> &nalUnit) {
    if (nalUnit.size() < 5) return -1;
    // NAL头第2字节的高6位为nal_unit_type
    return (nalUnit[4] & 0x7E) >> 1;
}
```

H.265 NAL单元类型定义：

```cpp
// h265_common.h
enum NalUnitType : uint8_t {
    TRAIL_N = 0,  TRAIL_R = 1,       // 普通Slice
    BLA_W_LP = 16, BLA_W_RADL = 17,  // 断链访问
    IDR_W_RADL = 19, IDR_N_LP = 20,  // IDR关键帧
    CRA_NUT = 21,                     // CRA关键帧
    VPS_NUT = 32,                     // 视频参数集
    SPS_NUT = 33,                     // 序列参数集
    PPS_NUT = 34,                     // 图像参数集
    AUD_NUT = 35,                     // 访问单元分隔符
    // ...
};
```

### 4.2 VPS/SPS/PPS提取（CSD-0）

`parseH265CSD0`方法遍历比特流，查找起始码并按NAL类型提取VPS、SPS、PPS，组合成Android MediaCodec需要的CSD-0（Codec Specific Data）：

```cpp
// HevcNalParse.cpp
std::pair<const void*, size_t> HevcNalParse::parseH265CSD0(
    std::vector<uint8_t> *bitstream_buffer) {

    std::vector<uint8_t> csd;
    bool isGetVsp = false, isGetSps = false, isGetPsp = false;
    size_t pos = 0;

    while (pos < stream.size()) {
        // 查找起始码 00 00 01 或 00 00 00 01
        // 提取NAL单元
        int nalUnitType = getNalUnitType(nalUnit);

        switch (nalUnitType) {
            case 32:  // VPS_NUT
                csd.insert(csd.end(), nalUnit.begin(), nalUnit.end());
                isGetVsp = true;
                break;
            case 33:  // SPS_NUT
                csd.insert(csd.end(), nalUnit.begin(), nalUnit.end());
                isGetSps = true;
                break;
            case 34:  // PPS_NUT
                csd.insert(csd.end(), nalUnit.begin(), nalUnit.end());
                isGetPsp = true;
                break;
        }

        // 收集完VPS+SPS+PPS就停止
        if (isGetVsp && isGetSps && isGetPsp) break;
        pos = nextStart;
    }

    return {csd.data(), csd.size()};
}
```

H.265的SPS直接给出像素分辨率（不需要像H.264那样乘以16）：

```cpp
// HevcNalParse.cpp
// 分辨率（H.265直接给出像素值）
uint32_t width =
    bitstream_parser_state->GetSps(0)->pic_width_in_luma_samples;
uint32_t height =
    bitstream_parser_state->GetSps(0)->pic_height_in_luma_samples;

// 帧率（H.265帧率公式，分母无2.0）
if (vui_parameters->vui_timing_info_present_flag) {
    float frameRate = timeScale / (1.0 * numUnitsInTick);
}
```

### 4.3 HEVC转MP4封装实现

`Hevc2MP4`使用Android NDK的`AMediaMuxer`将HEVC裸流封装为MP4容器：

```cpp
// Hevc2MP4.cpp
int Hevc2MP4::hevcConverterMp4(const char *inPath, const char *outPath) {
    // 1. 打开输入输出文件
    FILE *infp = fopen(inPath, "rb");
    FILE *outfp = fopen(outPath, "wb+");

    // 2. 配置封装器（解析SPS提取宽高/帧率，提取CSD-0）
    configMuxer(inPath);

    // 3. 逐NAL写入
    writeSampleData(bitstream_buffer);

    // 4. 释放资源
    release();
}

void Hevc2MP4::configMuxer(const char *inPath) {
    // 解析H.265比特流
    hevcNalParse->setHevcNalDataPath(inPath);

    // 从SPS提取宽高
    uint32_t width = spsState->pic_width_in_luma_samples;
    uint32_t height = spsState->pic_height_in_luma_samples;

    // 从VUI计算帧率
    uint32_t frameRate = timeScale / (1.0 * numUnitsInTick);

    // 创建MediaFormat
    m_AMediaFormat = AMediaFormat_new();
    AMediaFormat_setString(m_AMediaFormat, AMEDIAFORMAT_KEY_MIME, "video/hevc");
    AMediaFormat_setInt32(m_AMediaFormat, AMEDIAFORMAT_KEY_WIDTH, width);
    AMediaFormat_setInt32(m_AMediaFormat, AMEDIAFORMAT_KEY_HEIGHT, height);
    AMediaFormat_setInt32(m_AMediaFormat, AMEDIAFORMAT_KEY_FRAME_RATE, frameRate);

    // 设置CSD-0（VPS+SPS+PPS）
    auto csd_0 = hevcNalParse->parseH265CSD0(hevcNalParse->bitstream_buffer);
    AMediaFormat_setBuffer(m_AMediaFormat, "csd-0", csd_0.first, csd_0.second);

    // 创建MediaMuxer并添加轨道
    m_AMediaMuxer = AMediaMuxer_new(fileno(outfp),
                                    AMEDIAMUXER_OUTPUT_FORMAT_MPEG_4);
    videoTrackIndex = AMediaMuxer_addTrack(m_AMediaMuxer, m_AMediaFormat);
    AMediaMuxer_start(m_AMediaMuxer);
}

int Hevc2MP4::writeSampleData(std::vector<uint8_t> *bitstream_buffer) {
    while (pos < stream.size()) {
        // 查找起始码，确定NAL边界
        std::vector<uint8_t> nalUnit(stream.begin() + pos, stream.begin() + nextStart);

        // 判断是否关键帧（NAL类型16-23为IRAP帧）
        if (isKeyFrame(nalUnit)) {
            info.flags = AMEDIACODEC_CONFIGURE_FLAG_ENCODE;  // 标记为关键帧
        }

        // 计算PTS
        info.presentationTimeUs = computePresentationTime(frameIndex++);

        // 写入MediaMuxer
        AMediaMuxer_writeSampleData(m_AMediaMuxer, videoTrackIndex,
                                     nalUnit.data(), &info);
        pos = nextStart;
    }
}

bool Hevc2MP4::isKeyFrame(const std::vector<uint8_t> &nalUnit) {
    int nalUnitType = getNalUnitType(nalUnit);
    // NAL类型16-23为IRAP（Intra Random Access Point）帧
    return (nalUnitType >= 16 && nalUnitType <= 23);
}
```

---

## 五、RTMP直播推流模块（rtmplive）

`rtmplive`模块实现了完整的Android端RTMP直播推流功能，整合了Camera2视频采集、AudioRecord音频采集、x264视频编码、FAAC音频编码、librtmp推流。

### 5.1 推流架构总览

```
Java层:
  LiveManger (Camera2采集 + AudioRecord采集)
    └─ RtmpLivePusher (JNI桥接)
         └─ native方法

C++层:
  RtmpPusherManger (总管理器)
    ├─ VideoStreamPacket (x264编码 → RTMP Packet)
    ├─ AudioStreamPacket (FAAC编码 → RTMP Packet)
    └─ RtmpInit (RTMP连接 + 发送线程)
         └─ PacketQueue<RTMPPacket*> (线程安全队列)
```

JNI注册了11个native方法：

```cpp
// RtmpLiveJniCall.cpp
static const JNINativeMethod methods[] = {
    {"native_init_callback",     "()V",                   cpp_init_rtmp_callback},
    {"native_rtmp_init",         "()V",                   cpp_init_rtmp},
    {"native_rtmp_start_push",   "(Ljava/lang/String;)V", cpp_start_rtmp},
    {"native_rtmp_stop_push",    "()V",                   cpp_stop_rtmp},
    {"native_rtmp_pause",        "()V",                   cpp_pause_rtmp},
    {"native_rtmp_release",      "()V",                   cpp_release_rtmp},
    {"native_push_video_data",   "([B)V",                 cpp_push_video_data},
    {"native_video_codec_info",  "(IIII)V",               cpp_video_codec_info},
    {"native_getInputSamples",   "()I",                   cpp_audio_get_input_samples},
    {"native_setAudioCodecInfo", "(II)V",                 cpp_audio_set_audio_codec_info},
    {"native_push_audio_data",   "([B)V",                 cpp_push_audio_data},
};
```

### 5.2 x264视频编码与RTMP封装

`VideoStreamPacket`使用x264编码器将YUV420数据编码为H.264，再封装为RTMP Packet。

**x264编码器初始化**：

```cpp
// VideoStreamPacket.cpp
int VideoStreamPacket::setVideoEncInfo(int width, int height, int fps, int bitrate) {
    x264_param_t param;
    x264_param_default_preset(&param, "ultrafast", "zerolatency");  // 极速+零延迟
    param.i_level_idc = 32;
    param.i_csp = X264_CSP_I420;           // 输入格式I420
    param.i_width = width;
    param.i_height = height;
    param.i_bframe = 0;                     // 无B帧（降低延迟）
    param.rc.i_rc_method = X264_RC_ABR;     // 平均码率控制
    param.rc.i_bitrate = bitrate / 1024;
    param.i_fps_num = fps;
    param.i_keyint_max = fps * 2;           // GOP = 2秒
    param.b_repeat_headers = 1;            // 每个关键帧附带SPS/PPS
    x264_param_apply_profile(&param, "baseline");

    videoCodec = x264_encoder_open(&param);
    pic_in = new x264_picture_t();
    x264_picture_alloc(pic_in, X264_CSP_I420, width, height);
    return 0;
}
```

关键编码参数说明：
- `ultrafast` + `zerolatency`：最低延迟预设，适合实时推流
- `i_bframe = 0`：禁用B帧，避免编码缓冲带来的延迟
- `i_keyint_max = fps * 2`：GOP长度为2秒
- `baseline` profile：兼容性最好，适合RTMP推流

**YUV编码为H.264**：

```cpp
void VideoStreamPacket::encodeVideo(int8_t *data) {
    // YUV420三平面分离
    memcpy(pic_in->img.plane[0], data, m_frameLen);               // Y
    memcpy(pic_in->img.plane[1], data + offset, m_frameLen / 4);  // U
    memcpy(pic_in->img.plane[2], data + offset, m_frameLen / 4);  // V

    // x264编码
    x264_nal_t *pp_nal;
    int pi_nal;
    x264_encoder_encode(videoCodec, &pp_nal, &pi_nal, pic_in, &pic_out);

    // 遍历NAL，按类型封装为RTMP Packet
    for (int i = 0; i < pi_nal; ++i) {
        if (nal.i_type == NAL_SPS) {
            // 提取SPS
        } else if (nal.i_type == NAL_PPS) {
            // 提取PPS，发送SPS+PPS包（AVC Sequence Header）
            sendSpsPps(sps, pps, sps_len, pps_len);
        } else {
            // 发送视频帧
            sendFrame(nal.i_type, nal.p_payload, nal.i_payload);
        }
    }
}
```

**SPS/PPS封装为RTMP Packet**（AVC Sequence Header）：

```cpp
void VideoStreamPacket::sendSpsPps(uint8_t *sps, uint8_t *pps,
                                     int sps_len, int pps_len) {
    int bodySize = 13 + sps_len + 3 + pps_len;
    RTMPPacket_Alloc(packet, bodySize);

    int i = 0;
    // FrameType(4bit: KeyFrame=1) + CodecID(4bit: AVC=7)
    packet->m_body[i++] = 0x17;
    // AVCPacketType: 0 = AVC sequence header
    packet->m_body[i++] = 0x00;
    // CompositionTime: 3 bytes = 0
    packet->m_body[i++] = 0x00;
    packet->m_body[i++] = 0x00;
    packet->m_body[i++] = 0x00;

    // AVCDecoderConfigurationRecord
    packet->m_body[i++] = 0x01;       // configurationVersion
    packet->m_body[i++] = sps[1];      // AVCProfileIndication
    packet->m_body[i++] = sps[2];      // profile_compatibility
    packet->m_body[i++] = sps[3];      // AVCLevelIndication
    packet->m_body[i++] = 0xFF;        // lengthSizeMinusOne = 3 (4 bytes)

    // SPS
    packet->m_body[i++] = 0xE1;        // numOfSequenceParameterSets = 1
    packet->m_body[i++] = (sps_len >> 8) & 0xFF;  // SPS length (high)
    packet->m_body[i++] = sps_len & 0xFF;          // SPS length (low)
    memcpy(&packet->m_body[i], sps, sps_len);
    i += sps_len;

    // PPS
    packet->m_body[i++] = 0x01;        // numOfPictureParameterSets = 1
    packet->m_body[i++] = (pps_len >> 8) & 0xFF;  // PPS length (high)
    packet->m_body[i++] = pps_len & 0xFF;          // PPS length (low)
    memcpy(&packet->m_body[i], pps, pps_len);

    // Packet类型设置
    packet->m_packetType = RTMP_PACKET_TYPE_VIDEO;
    packet->m_nChannel = 0x04;
    packet->m_headerType = RTMP_PACKET_SIZE_MEDIUM;
}
```

**视频帧封装为RTMP Packet**：

```cpp
void VideoStreamPacket::sendFrame(int type, uint8_t *payload, int i_payload) {
    // 去除起始码 (00 00 00 01 或 00 00 01)
    if (payload[2] == 0x00) {
        i_payload -= 4;
        payload += 4;
    } else {
        i_payload -= 3;
        payload += 3;
    }

    int bodySize = 5 + i_payload + 4;  // header(5) + length(4) + data
    RTMPPacket_Alloc(packet, bodySize);

    int i = 0;
    // FrameType: KeyFrame(0x17) or InterFrame(0x27) + CodecID: AVC(7)
    if (type == NAL_SLICE_IDR) {
        packet->m_body[i++] = 0x17;  // Key frame + AVC
    } else {
        packet->m_body[i++] = 0x27;  // Inter frame + AVC
    }
    // AVCPacketType: 1 = AVC NALU
    packet->m_body[i++] = 0x01;
    // CompositionTime: 3 bytes = 0
    packet->m_body[i++] = 0x00;
    packet->m_body[i++] = 0x00;
    packet->m_body[i++] = 0x00;

    // NALU length (4 bytes, big-endian)
    packet->m_body[i++] = (i_payload >> 24) & 0xFF;
    packet->m_body[i++] = (i_payload >> 16) & 0xFF;
    packet->m_body[i++] = (i_payload >> 8) & 0xFF;
    packet->m_body[i++] = i_payload & 0xFF;

    // NALU data
    memcpy(&packet->m_body[i], payload, i_payload);

    packet->m_packetType = RTMP_PACKET_TYPE_VIDEO;
    packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
}
```

### 5.3 FAAC音频编码与RTMP封装

`AudioStreamPacket`使用FAAC编码器将PCM数据编码为AAC，再封装为RTMP Packet。

**FAAC编码器初始化**：

```cpp
int AudioStreamPacket::setAudioEncInfo(int samplesInHZ, int channels) {
    m_audioCodec = faacEncOpen(samplesInHZ, channels,
                                &m_inputSamples, &m_maxOutputBytes);
    faacEncConfigurationPtr config = faacEncGetCurrentConfiguration(m_audioCodec);
    config->mpegVersion = MPEG4;
    config->aacObjectType = LOW;           // AAC-LC
    config->inputFormat = FAAC_INPUT_16BIT; // 16位PCM输入
    config->outputFormat = 0;               // Raw AAC（无ADTS头，RTMP要求）
    faacEncSetConfiguration(m_audioCodec, config);
    return 0;
}
```

**音频帧封装**：

```cpp
void AudioStreamPacket::encodeData(int8_t *data) {
    int byteLen = faacEncEncode(m_audioCodec, (int32_t *) data,
                                 m_inputSamples, m_buffer, m_maxOutputBytes);
    if (byteLen > 0) {
        int bodySize = 2 + byteLen;
        RTMPPacket_Alloc(packet, bodySize);

        // 音频头字节
        // SoundFormat(4bit: AAC=10) + SoundRate(2bit: 44.1kHz=11)
        // + SoundSize(1bit: 16bit=1) + SoundType(1bit: Stereo=11)
        packet->m_body[0] = (channels == 1) ? 0xAE : 0xAF;
        // 0xAF = 1010 1111: AAC + 44.1kHz + 16bit + Stereo
        // 0xAE = 1010 1110: AAC + 44.1kHz + 16bit + Mono

        // AACPacketType: 1 = AAC raw data
        packet->m_body[1] = 0x01;

        memcpy(&packet->m_body[2], m_buffer, byteLen);

        packet->m_packetType = RTMP_PACKET_TYPE_AUDIO;
        packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
    }
}
```

**AAC Sequence Header**（AudioSpecificConfig）：

```cpp
RTMPPacket *AudioStreamPacket::getAudioTag() {
    // 获取AudioSpecificConfig
    unsigned char *buf;
    unsigned long len;
    faacEncGetDecoderSpecificInfo(m_audioCodec, &buf, &len);

    RTMPPacket_Alloc(packet, 2 + len);
    packet->m_body[0] = 0xAF;  // 同上音频头
    // AACPacketType: 0 = AAC sequence header
    packet->m_body[1] = 0x00;
    memcpy(&packet->m_body[2], buf, len);

    return packet;
}
```

### 5.4 RTMP连接与发送线程

`RtmpInit`负责RTMP协议的连接建立和数据发送，在独立线程中运行：

```cpp
// RtmpInit.cpp
void RtmpInit::startThread() {
    // 1. 创建RTMP对象并初始化
    RTMP *rtmp = RTMP_Alloc();
    RTMP_Init(rtmp);
    rtmp->Link.timeout = 5;  // 超时5秒

    // 2. 设置推流URL
    RTMP_SetupURL(rtmp, url);
    RTMP_EnableWrite(rtmp);   // 启用推流（输出）模式

    // 3. 建立连接
    RTMP_Connect(rtmp, nullptr);       // TCP连接
    RTMP_ConnectStream(rtmp, 0);       // RTMP流连接

    // 4. 发送循环
    while (isPushing) {
        RTMPPacket *packet = nullptr;
        packetQueue.pop(packet);  // 阻塞等待数据包

        if (packet) {
            packet->m_nInfoField2 = rtmp->m_stream_id;
            RTMP_SendPacket(rtmp, packet, 1);  // 发送
            releasePackets(packet);             // 释放
        }
    }

    // 5. 关闭连接
    RTMP_Close(rtmp);
    RTMP_Free(rtmp);
}
```

时间戳计算使用相对于推流开始时间的偏移：

```cpp
void RtmpInit::addRtmpPacket(RTMPPacket *packet) {
    // 计算相对时间戳（毫秒）
    packet->m_nTimeStamp = RTMP_GetTime() - start_time;
    packetQueue.push(packet);
}
```

### 5.5 线程安全的生产者-消费者队列

`PacketQueue`是模板类，使用`std::mutex` + `std::condition_variable`实现线程安全的生产者-消费者模型，解决编码线程和发送线程之间的数据传递问题：

```cpp
// PacketQueue.h
template<typename T>
class PacketQueue {
private:
    std::mutex m_mutex;
    std::condition_variable m_cond;
    std::queue<T> m_queue;
    bool m_running;

public:
    void push(T value) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push(value);
        m_cond.notify_one();  // 通知消费者
    }

    bool pop(T &value) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.wait(lock, [this] {
            return !m_queue.empty() || !m_running;
        });
        if (!m_running && m_queue.empty()) return false;
        value = m_queue.front();
        m_queue.pop();
        return true;
    }

    void setRunning(bool running) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_running = running;
        m_cond.notify_all();  // 唤醒所有等待线程
    }

    void clear() {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (!m_queue.empty()) m_queue.pop();
    }
};
```

**Java层Camera2采集与推流**：

```java
// LiveManger.java
public class LiveManger {
    private RtmpLivePusher mRtmpLivePusher;
    private RtmpCamera2Helper mCameraHelper;

    // Camera2预览帧回调
    private RtmpCamera2Helper.OnPreviewFrame onPreviewFrame = (yuvData, width, height) -> {
        // 推送视频数据到C++编码
        mRtmpLivePusher.pushVideoData(yuvData);
    };

    // 音频采集线程
    private void AudioTask() {
        AudioRecord audioRecord = new AudioRecord(...);
        audioRecord.startRecording();
        while (isRecording) {
            audioRecord.read(buffer, 0, bufferSize);
            // 推送音频数据到C++编码
            mRtmpLivePusher.pushAudioData(buffer);
        }
    }
}
```

Camera2采集通过`ImageReader`获取`YUV_420_888`格式帧，再转换为I420紧凑格式：

```java
// RtmpCamera2Helper.java
private class OnImageAvailableListenerImpl implements ImageReader.OnImageAvailableListener {
    @Override
    public void onImageAvailable(ImageReader reader) {
        Image image = reader.acquireLatestImage();
        // YUV_420_888 → I420 转换
        byte[] yuvData = YUVUtil.convertYUV420888ToI420(image);
        // 回调到LiveManger
        listener.onPreviewFrame(yuvData, width, height);
    }
}
```

---

## 技术亮点与总结

本项目在Android音视频开发领域做了非常全面的实践，涵盖从底层码流解析到上层渲染显示的完整链路。以下是核心技术亮点：

**1. 统一的JNI动态注册架构**

所有模块统一使用`RegisterNatives`动态注册JNI方法，在`JNI_OnLoad`中完成映射。相比静态注册，动态注册避免了方法名耦合，执行效率更高，也便于维护。C++子线程通过`AttachCurrentThread` + `NewGlobalRef`实现回调Java，是跨线程通信的经典模式。

**2. FFmpeg新API的完整实践**

视频解码使用`avcodec_send_packet` + `avcodec_receive_frame`分离式API，取代了已废弃的`avcodec_decode_video2`。解码后通过`sws_scale`将YUV转换为RGBA，再通过`ANativeWindow`渲染到屏幕。播放控制通过`std::condition_variable`和`std::atomic_bool`实现了线程安全的暂停/停止/Seek。

**3. 丰富的OpenGL ES渲染能力**

13种滤镜 + 5种FBO后处理效果覆盖了常见的图像处理算法，从简单的均值模糊到复杂的卡通化（HSV量化 + Sobel边缘检测）。3D光照模型完整实现了Phong着色（环境光 + 漫反射 + 高光 + 距离衰减 + 聚光锥体）。FreeType文字渲染和MediaCodec视频录制进一步扩展了渲染能力。

**4. H.264/H.265码流深度解析**

基于Facebook开源的h264nal/h265nal库，实现了按H.264/H.265标准的逐字段解析。正确处理了两种标准的差异：H.264 NAL头为1字节、分辨率需乘以16、帧率公式分母有2.0系数；H.265 NAL头为2字节、分辨率直接给出、帧率公式无2.0系数。

**5. 完整的RTMP推流链路**

从Camera2/AudioRecord采集 → x264/FAAC编码 → RTMP Packet封装 → librtmp发送，实现了端到端的直播推流。编码参数针对低延迟场景优化（`ultrafast` + `zerolatency` + 无B帧 + 2秒GOP），通过`PacketQueue`模板类解决了编码线程与发送线程的并发问题。

**6. 模块化设计与独立编译**

7个Gradle模块各自独立编译为`.so`动态库，C++层不存在代码共享，通过JNI接口暴露给Java层。这种设计使得各功能模块可以独立复用，也便于按需裁剪。

> 项目GitHub地址：[https://github.com/wangyongyao1989/WyFFmpeg](https://github.com/wangyongyao1989/WyFFmpeg)
>
> 更多技术内容请关注作者CSDN博客：[https://blog.csdn.net/wangyongyao1989](https://blog.csdn.net/wangyongyao1989)
