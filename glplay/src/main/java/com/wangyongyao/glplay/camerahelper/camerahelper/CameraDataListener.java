package com.wangyongyao.glplay.camerahelper.camerahelper;

/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2024/9/3 16:14
 * Descibe : MyyFFmpeg com.wangyongyao.glplay.camerahelper.camerahelper
 */
public interface CameraDataListener {
    void onPreviewFrame(byte[] yuvData, int width, int height);
}
