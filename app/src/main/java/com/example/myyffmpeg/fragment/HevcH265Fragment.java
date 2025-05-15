package com.example.myyffmpeg.fragment;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.lifecycle.ViewModelProviders;

import com.example.myyffmpeg.FFViewModel;
import com.example.myyffmpeg.databinding.FragmentHevcH265LayoutBinding;
import com.example.myyffmpeg.utils.DirectoryPath;
import com.example.myyffmpeg.utils.FileUtils;
import com.wangyongyao.common.utils.ToastManager;
import com.wangyongyao.h265.H265CallJni;
import com.wangyongyao.h265.nal.HevcToMp4Converter;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2025/3/14 14:40
 * Descibe : MyyFFmpeg com.example.myyffmpeg.fragment
 */
public class HevcH265Fragment extends BaseFragment {

    private static final String TAG = HevcH265Fragment.class.getSimpleName();
    private com.example.myyffmpeg.databinding.FragmentHevcH265LayoutBinding mBinding;
    private Button mH265Back;
    private FFViewModel mFfViewModel;
    private Button mBtnHecToMp4;
    private H265CallJni mH265CallJni;
    private Button mBtnHevc1;


    @Override
    public View getLayoutDataBing(@NonNull LayoutInflater inflater
            , @Nullable ViewGroup container
            , @Nullable Bundle savedInstanceState) {
        mBinding = FragmentHevcH265LayoutBinding.inflate(inflater);
        return mBinding.getRoot();
    }

    @Override
    public void initView() {
        mH265Back = mBinding.btnHevcH265Back;
        mBtnHecToMp4 = mBinding.btnHevcToMp4;
        mBtnHevc1 = mBinding.btnHevc1;
    }

    @Override
    public void initData() {
        mH265CallJni = new H265CallJni();
    }

    @Override
    public void initObserver() {
        mFfViewModel = ViewModelProviders.of(requireActivity())
                .get(FFViewModel.class);
    }

    @Override
    public void initListener() {
        mH265Back.setOnClickListener(view -> {
            mFfViewModel.getSwitchFragment().postValue(FFViewModel.FRAGMENT_STATUS.MAIN);
        });
        mBtnHecToMp4.setOnClickListener(view -> {
            HevcToMP4();
        });

        mBtnHevc1.setOnClickListener(view -> {
//            String h265FilePath = FileUtils.getModelFilePath(getActivity(), "codec1.h265");
            String h265FilePath = FileUtils.getModelFilePath(getActivity(), "output.h265");
            mH265CallJni.testH265(h265FilePath);
            ToastManager.getInstance(getContext()).showToast("解析Hevc数据Nal数据",0);

        });

    }

    private void HevcToMP4() {
        @SuppressLint("SimpleDateFormat") SimpleDateFormat formatter
                = new SimpleDateFormat("yy_MM_dd_HH_mm_ss");
        Date curDate = new Date(System.currentTimeMillis());//获取当前时间
        String str = formatter.format(curDate);
        String videoDir = DirectoryPath.createVideoDir(getActivity());
        String videoName = videoDir + File.pathSeparator + str + ".mp4";

//        String h265FilePath = FileUtils.getModelFilePath(getActivity(), "output.h265");
        String h265FilePath = FileUtils.getModelFilePath(getActivity(), "codec1.h265");
//        String h265FilePath = FileUtils.getModelFilePath(getActivity(), "output.h265");
        mH265CallJni.Hevc2MP4(h265FilePath, videoName);


//        String h265FilePath = FileUtils.getModelFilePath(getActivity(), "output.h265");
//        HevcToMp4Converter hevcToMp4Converter = new HevcToMp4Converter();
//        hevcToMp4Converter.convert(h265FilePath, videoName);
//        H265ToMp4Converter.convert(h265FilePath, videoName);

    }
}
