package com.wangyongyao.glplay.camera;


import android.util.Size;

public interface GLCamera2Listener {

    void onCameraOpened(Size previewSize, int displayOrientation);

    void onPreviewFrame(byte[] yuvData, int width, int height);

    void onCameraClosed();

    void onCameraError(Exception e);

}
