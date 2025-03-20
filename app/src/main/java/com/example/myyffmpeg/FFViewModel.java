package com.example.myyffmpeg;

import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

public class FFViewModel extends ViewModel {


    private final MutableLiveData<FRAGMENT_STATUS> switchFragment
            = new MutableLiveData<>();

    public enum FRAGMENT_STATUS {
        MAIN,
        PLAY,
        RTMP,
        OPENGL_CAMERA,
        OPENGL_CAMERA_FBO,
        HEVC_H265,
        H264,

    }


    public MutableLiveData<FRAGMENT_STATUS> getSwitchFragment() {
        return switchFragment;
    }
}
