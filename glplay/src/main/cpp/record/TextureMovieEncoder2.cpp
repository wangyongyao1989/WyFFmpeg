//  Author : wangyongyao https://github.com/wangyongyao1989
// Created by MMM on 2024/10/15.
//

#include "../includeopengl/TextureMovieEncoder2.h"

TextureMovieEncoder2::TextureMovieEncoder2(VideoEncoderCore *videoEncoderCore) {
    m_VideoEncoderCore = videoEncoderCore;

}

TextureMovieEncoder2::~TextureMovieEncoder2() {
    // 必须先 quit() 再释放 codec。quit() 会 join 编码器线程，保证队列里已经投递的
    // MSG_STOP_RECORDING / MSG_FRAME_AVAILABLE 在 codec 还存活时执行完。
    // 原顺序是先 release()（把 m_AMediaCodec 置空）再 quit()，编码器线程随后在
    // drainEncoder 里对空 codec 调 AMediaCodec_dequeueOutputBuffer，
    // 真机点击"停止录制"必崩：SIGSEGV SEGV_MAPERR fault addr 0x0。
    quit();
    if (m_VideoEncoderCore != nullptr) {
        m_VideoEncoderCore->release();
        m_VideoEncoderCore = nullptr;
    }
}


void TextureMovieEncoder2::stopRecording() {
    postMessage(MSG_STOP_RECORDING);
}

bool TextureMovieEncoder2::isRecording() {

    return false;
}

void TextureMovieEncoder2::frameAvailableSoon() {
//    LOGE("TextureMovieEncoder2::frameAvailableSoon");
    postMessage(MSG_FRAME_AVAILABLE);
}

void TextureMovieEncoder2::handleFrameAvailable() {
//    LOGE("TextureMovieEncoder2::handleMessage handleFrameAvailable");
    m_VideoEncoderCore->drainEncoder(false);
}

void TextureMovieEncoder2::handleStopRecording() {
    LOGE("TextureMovieEncoder2::handleMessage handleStopRecording");
    m_VideoEncoderCore->drainEncoder(true);
    m_VideoEncoderCore->release();
}


void TextureMovieEncoder2::handleMessage(LooperMessage *msg) {
    Looper::handleMessage(msg);
    switch (msg->what) {
        case MSG_STOP_RECORDING: {
//            LOGE("TextureMovieEncoder2::handleMessage MSG_STOP_RECORDING");
            handleStopRecording();
        }
            break;
        case MSG_FRAME_AVAILABLE:
//            LOGE("TextureMovieEncoder2::handleMessage MSG_FRAME_AVAILABLE");
            handleFrameAvailable();
            break;
        default:
            break;
    }
}
