package com.wangyongyao.h264.camera;

import android.util.Size;

/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2025/3/31 16:36
 * Descibe : MyyFFmpeg com.wangyongyao.h264.camera
 */
public interface X264Camera2Listener {
    void onCameraOpened(Size previewSize, int displayOrientation);

    void onPreviewFrame(byte[] yuvData, int width, int height);

    void onCameraClosed();

    void onCameraError(Exception e);
}
