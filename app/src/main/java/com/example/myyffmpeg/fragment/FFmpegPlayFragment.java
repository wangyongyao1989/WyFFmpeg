package com.example.myyffmpeg.fragment;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.RelativeLayout;
import android.widget.SeekBar;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.lifecycle.ViewModelProviders;

import com.example.myyffmpeg.CallJni;
import com.example.myyffmpeg.FFViewModel;
import com.example.myyffmpeg.databinding.FragmentPlayLayoutBinding;
import com.example.myyffmpeg.utils.DirectoryPath;
import com.example.myyffmpeg.utils.FileUtils;
import com.example.myyffmpeg.utils.SaveMediaUtils;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;

public class FFmpegPlayFragment extends BaseFragment {

    private FragmentPlayLayoutBinding mBinding;
    private Button mBtnGetVersion;
    private Button mBtnMp4Avi;
    private Button mAddWaterMark;
    private Button mBtnPlayAudio;
    private Button mBtnPlayPauseVideo;
    private Button mBtnPlayVideoStop;
    private CallJni mCallJni;

    private String mVideoUrl;

    private SurfaceView mSurfaceView = null;
    private Surface mSurface = null;
    private SeekBar mSeekBar;
    private TextView mSampleText;
    private FFViewModel mFfViewModel;
    private RelativeLayout mRlClose;

    @Override
    public View getLayoutDataBing(@NonNull LayoutInflater inflater
            , @Nullable ViewGroup container
            , @Nullable Bundle savedInstanceState) {
        mBinding = FragmentPlayLayoutBinding.inflate(inflater);
        return mBinding.getRoot();
    }



    @Override
    public void initView() {
        mRlClose = mBinding.rlClose;
        mSampleText = mBinding.sampleText;
        mBtnGetVersion = mBinding.btnGetVersion;
        mBtnMp4Avi = mBinding.btnMp4Avi;
        mAddWaterMark = mBinding.btnAddWaterMark;
        mBtnPlayAudio = mBinding.btnPlayAudio;
        mBtnPlayPauseVideo = mBinding.btnPlayPauseVideo;
        mBtnPlayVideoStop = mBinding.btnPlayVideoStop;
        mSurfaceView = mBinding.surfacePlay;
        mSeekBar = mBinding.sbPlay;
    }

    @Override
    public void initData() {
        mCallJni = new CallJni();
        mVideoUrl = FileUtils.getModelFilePath(getActivity(), "midway.mp4");
    }

    @Override
    public void initObserver() {
        mFfViewModel = ViewModelProviders.of(requireActivity())
                .get(FFViewModel.class);
    }

    @SuppressLint("SetTextI18n")
    @Override
    public void initListener() {
        mRlClose.setOnClickListener(view -> {
            mFfViewModel.getSwitchFragment().postValue(FFViewModel.FRAGMENT_STATUS.MAIN);
        });

        mBtnGetVersion.setOnClickListener(view -> {
            mSampleText.setText(mCallJni.callStringForJNI()
                    + mCallJni.callIntForJNI()
                    + mCallJni.getFFmpegVersion());
        });
        
        mBtnMp4Avi.setOnClickListener(view -> {
            String inputFilePath = FileUtils.getModelFilePath(getActivity(), "woman.mp4");
            @SuppressLint("SimpleDateFormat") SimpleDateFormat formatter
                    = new SimpleDateFormat("yy_MM_dd_HH_mm_ss");
            Date curDate = new Date(System.currentTimeMillis());//获取当前时间
            String str = formatter.format(curDate);
            String aviDir = DirectoryPath.createVideoDir(getActivity());
            String aviName = aviDir + File.pathSeparator + str + ".avi";
            mCallJni.mp4CAvi(inputFilePath, aviName);
            SaveMediaUtils.saveImgFileToAlbum(getActivity(), aviName);
        });
        mAddWaterMark.setOnClickListener(view -> {
            String inputFilePath = FileUtils.getModelFilePath(getActivity(), "video.mp4");
            String pngFilePath = FileUtils.getModelFilePath(getActivity(), "water.png");
            String waterMakerDir = DirectoryPath.createVideoDir(getActivity());
            String waterMakerFilePath = waterMakerDir + File.pathSeparator + "watermark.mp4";
            mCallJni.waterMark(inputFilePath, pngFilePath, waterMakerFilePath);
        });
        mBtnPlayAudio.setOnClickListener(view -> {
            if (mBtnPlayAudio.isSelected()) {
                mCallJni.stopAudio();
                mBtnPlayAudio.setSelected(false);
                mBtnPlayAudio.setText("音频播放");
            } else {
                String audioUrl = FileUtils.getModelFilePath(getActivity(), "liudehua.mp3");
                mCallJni.playAudio(audioUrl);
                mBtnPlayAudio.setSelected(true);
                mBtnPlayAudio.setText("停止音频播放");
            }
        });
        mBtnPlayPauseVideo.setOnClickListener(view -> {
            if (mBtnPlayPauseVideo.isSelected()) {
                mBtnPlayPauseVideo.setSelected(false);
                mBtnPlayPauseVideo.setText("ffmpeg播放视频");
                mCallJni.pauseVideo();
            } else {
                mBtnPlayPauseVideo.setSelected(true);
                mBtnPlayPauseVideo.setText("ffmpeg播放暂停");
                mCallJni.playVideo();
            }
        });

        mBtnPlayVideoStop.setOnClickListener(view -> {
            if (mBtnPlayVideoStop.isSelected()) {
                mBtnPlayVideoStop.setSelected(false);
                mBtnPlayVideoStop.setText("播放视频");
                mCallJni.stopVideo();
            } else {
                String videoUrl = FileUtils.getModelFilePath(getActivity(), "woman.mp4");
                mBtnPlayVideoStop.setSelected(true);
                mBtnPlayVideoStop.setText("播放停止");
                mCallJni.playVideo();
            }
        });

        final SurfaceHolder surfaceViewHolder = mSurfaceView.getHolder();
        surfaceViewHolder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder holder) {
                mSurface = surfaceViewHolder.getSurface();
                mCallJni.initPlay(mVideoUrl, mSurface);
                mCallJni.initCallBackListener();
            }

            @Override
            public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {

            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
                mCallJni.unInitPlay();
            }
        });

        mSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                mCallJni.seekToPosition(mSeekBar.getProgress());
            }
        });
    }


}
