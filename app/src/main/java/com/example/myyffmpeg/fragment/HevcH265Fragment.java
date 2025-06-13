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
import com.example.myyffmpeg.converter.VideoConfig;
import com.example.myyffmpeg.converter.VideoSaver;
import com.example.myyffmpeg.databinding.FragmentHevcH265LayoutBinding;
import com.example.myyffmpeg.utils.StringToIntConverter;
import com.wangyongyao.common.utils.DirectoryPath;
import com.wangyongyao.common.utils.FileUtils;
import com.wangyongyao.common.utils.ToastManager;
import com.wangyongyao.h265.H265CallJni;
import com.wangyongyao.h265.nal.H265ToMp4Muxer;


import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

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
    private VideoSaver mVideoSaver;
    private String mVideoName;
    private H265ToMp4Muxer mToMp4Muxer;


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
        mVideoName = videoDir + File.pathSeparator + str + ".mp4";

//        String h265FilePath = FileUtils.getModelFilePath(getActivity(), "output.h265");
        String h265FilePath = FileUtils.getModelFilePath(getActivity(), "codec1.h265");
//        String h265FilePath = FileUtils.getModelFilePath(getActivity(), "output.h265");
//        mH265CallJni.Hevc2MP4(h265FilePath, videoName);
//        mToMp4Muxer = new H265ToMp4Muxer(null);

        if (mVideoSaver == null) {
            VideoConfig videoConfig = new VideoConfig(VideoConfig.MIME_H265, 1280, 720);
            mVideoSaver = new VideoSaver(videoDir, videoConfig, "mm");
        }

//        String h265FilePath = FileUtils.getModelFilePath(getActivity(), "output.h265");
//        HevcToMp4Converter hevcToMp4Converter = new HevcToMp4Converter();
//        hevcToMp4Converter.convert(h265FilePath, videoName);
//        H265ToMp4Converter.convert(h265FilePath, videoName);

        getActivity().runOnUiThread(() -> {
            readFileInChunks(h265FilePath);

            if (mVideoSaver != null) {
                mVideoSaver.stop();
                mVideoSaver = null;
            }
        });
    }

    /**
     * 以2KB块的形式读取文件内容
     * @param filePath 文件路径
     * @return 完整的文件字节数组，如果出错则返回null
     */
    public byte[] readFileInChunks(String filePath) {
        final int BUFFER_SIZE = 10 * 1024; // 2KB缓冲区

        List<byte[]> chunkList = new ArrayList<>();
        int totalBytes = 0;

        try (FileInputStream fis = new FileInputStream(filePath)) {
            byte[] buffer = new byte[BUFFER_SIZE];
            int bytesRead;

            // 分块读取文件
            while ((bytesRead = fis.read(buffer)) != -1) {
                byte[] chunk = new byte[bytesRead];
                System.arraycopy(buffer, 0, chunk, 0, bytesRead);
//                String s = StringToIntConverter.bytes2hex(chunk);
//                Log.e(TAG, "s: "+s );
                chunkList.add(chunk);
                if (mVideoSaver != null) {
                    Log.e(TAG, "分块读取文件: " + chunkList.size());
//                    if (chunkList.size() < 10)
                    mVideoSaver.writeData(buffer,0,0,false);
                }

//                mToMp4Muxer.writeSample(chunk);

                totalBytes += bytesRead;
            }

            // 合并所有块到单个字节数组
            byte[] result = new byte[totalBytes];
            int destPos = 0;
            for (byte[] chunk : chunkList) {
                System.arraycopy(chunk, 0, result, destPos, chunk.length);
                destPos += chunk.length;
            }
            return result;

        } catch (Exception e) {
            Log.e(TAG, "Exception: " + e.getMessage());
            return null;
        }
    }



}

