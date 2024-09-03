package com.wangyongyao.glplay.camerahelper.camerahelper;

/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2024/9/3 10:58
 * Descibe : MyyFFmpeg com.example.myyffmpeg.utils
 */
public interface PreviewFrameHandler {
    void onPreviewFrame(byte[] data, int width, int height);
}
