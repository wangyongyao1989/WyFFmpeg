package com.example.myyffmpeg.fragment;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.lifecycle.ViewModelProviders;

import com.example.myyffmpeg.FFViewModel;
import com.example.myyffmpeg.databinding.FragmentRtmpLayoutBinding;
import com.example.rtmplive.LiveManger;

public class RtmpFragment extends BaseFragment {

    private static final String TAG = "RtmpFragment";
    /// 默认本地测试地址（配合 adb reverse tcp:1935 + 本机 node-media-server 验证）；
    /// 不再硬编码第三方推流地址与密钥。
    private static final String DEFAULT_RTMP_URL = "rtmp://127.0.0.1:1935/live/test";
    private static final String SP_NAME = "rtmp_config";
    private static final String SP_KEY_URL = "push_url";

    private FragmentRtmpLayoutBinding mBinding;
    private FFViewModel mFfViewModel;
    private LiveManger mLiveManger;
    private boolean isPushing = false;
    private String rtmpPushPath;

    @Override
    public View getLayoutDataBing(@NonNull LayoutInflater inflater
            , @Nullable ViewGroup container
            , @Nullable Bundle savedInstanceState) {
        mBinding = FragmentRtmpLayoutBinding.inflate(inflater, container, false);
        return mBinding.getRoot();
    }

    @Override
    public void initView() {
    }

    @Override
    public void initData() {
        mLiveManger = new LiveManger(this.getViewLifecycleOwner(),getContext(), mBinding.textureView);
        rtmpPushPath = requireContext()
                .getSharedPreferences(SP_NAME, android.content.Context.MODE_PRIVATE)
                .getString(SP_KEY_URL, DEFAULT_RTMP_URL);
        mBinding.etRtmpUrl.setText(rtmpPushPath);
    }

    @Override
    public void initObserver() {
        mFfViewModel = ViewModelProviders.of(requireActivity())
                .get(FFViewModel.class);
    }

    @Override
    public void initListener() {
        mBinding.btnRtmpBack.setOnClickListener(view -> {
            mLiveManger.releaseRtmp();
            mFfViewModel.getSwitchFragment().postValue(FFViewModel.FRAGMENT_STATUS.MAIN);
        });

        mBinding.btnFfPush.setOnClickListener(view -> {
            // 推流地址取自输入框并持久化，供下次使用
            String input = mBinding.etRtmpUrl.getText().toString().trim();
            if (input.startsWith("rtmp://")) {
                rtmpPushPath = input;
                requireContext().getSharedPreferences(SP_NAME, android.content.Context.MODE_PRIVATE)
                        .edit().putString(SP_KEY_URL, rtmpPushPath).apply();
            }
            if (!rtmpPushPath.startsWith("rtmp://")) {
                com.wangyongyao.common.utils.ToastManager.getInstance(getContext())
                        .showToast("请先填写有效的 rtmp:// 地址", 0);
                return;
            }
            mLiveManger.startRtmpPush(rtmpPushPath);
            isPushing = true;
        });

        mBinding.btnFfStop.setOnClickListener(view -> {
            mLiveManger.stopRtmpPush();
            isPushing = false;
        });

        mBinding.btnFfPause.setOnClickListener(view -> {
            mLiveManger.pauseRtmp();
        });
    }


    /**
     * MainActivity 用 hide()/show() 切换 Fragment，离开本页时 onDestroy 不会触发；
     * 推流中直接切走会留下摄像头/麦克风/编码器与 socket 一直被占用。
     */
    @Override
    public void onHiddenChanged(boolean hidden) {
        super.onHiddenChanged(hidden);
        if (hidden && mLiveManger != null) {
            if (isPushing) {
                mLiveManger.stopRtmpPush();
                isPushing = false;
            }
            mLiveManger.releaseRtmp();
        }
    }

    @Override
    public void onDestroy() {
        if (mLiveManger != null) {
            if (isPushing) {
                mLiveManger.stopRtmpPush();
            }
            mLiveManger.releaseRtmp();
            mLiveManger.release();
        }
        super.onDestroy();
    }
}
