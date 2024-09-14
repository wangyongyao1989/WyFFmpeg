package com.example.rtmplive.camera;


import android.util.Size;

public interface RtmpCamera2Listener {

    void onCameraOpened(Size previewSize, int displayOrientation);

    void onPreviewFrame(byte[] yuvData, int width, int height);

    void onCameraClosed();

    void onCameraError(Exception e);

}
