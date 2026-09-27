package com.example.myyffmpeg.fragment;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.lifecycle.ViewModelProviders;

import com.example.myyffmpeg.FFViewModel;
import com.example.myyffmpeg.converter.VideoConfig;
import com.example.myyffmpeg.converter.VideoSaver;
import com.example.myyffmpeg.databinding.FragmentHevcH265LayoutBinding;
import com.wangyongyao.common.utils.DirectoryPath;
import com.wangyongyao.common.utils.FileUtils;
import com.wangyongyao.common.utils.ToastManager;
import com.wangyongyao.h265.H265CallJni;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Date;

/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2025/3/14 14:40
 * Descibe : MyyFFmpeg com.example.myyffmpeg.fragment
 */
public class HevcH265Fragment extends BaseFragment {

    private static final String TAG = HevcH265Fragment.class.getSimpleName();
    private static final long FRAME_STEP_US = 1000000L / 30;
    private com.example.myyffmpeg.databinding.FragmentHevcH265LayoutBinding mBinding;
    private FFViewModel mFfViewModel;
    private H265CallJni mH265CallJni;
    private volatile boolean mConverting = false;


    @Override
    public View getLayoutDataBing(@NonNull LayoutInflater inflater
            , @Nullable ViewGroup container
            , @Nullable Bundle savedInstanceState) {
        mBinding = FragmentHevcH265LayoutBinding.inflate(inflater, container, false);
        return mBinding.getRoot();
    }

    @Override
    public void initView() {
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
        mBinding.btnHevcH265Back.setOnClickListener(view -> {
            mFfViewModel.getSwitchFragment().postValue(FFViewModel.FRAGMENT_STATUS.MAIN);
        });
        mBinding.btnHevcToMp4.setOnClickListener(view -> {
            HevcToMP4();
        });

        mBinding.btnHevc1.setOnClickListener(view -> {
            String h265FilePath = FileUtils.getModelFilePath(getActivity(), "output.h265");
            mH265CallJni.testH265(h265FilePath);
            ToastManager.getInstance(getContext()).showToast("解析Hevc数据Nal数据",0);

        });

    }

    private void HevcToMP4() {
        if (mConverting) {
            ToastManager.getInstance(getContext()).showToast("正在转换中，请稍候", 0);
            return;
        }
        @SuppressLint("SimpleDateFormat") SimpleDateFormat formatter
                = new SimpleDateFormat("yy_MM_dd_HH_mm_ss");
        Date curDate = new Date(System.currentTimeMillis());//获取当前时间
        String str = formatter.format(curDate);
        String videoDir = DirectoryPath.createVideoDir(getActivity());

        String h265FilePath = FileUtils.getModelFilePath(getActivity(), "codec1.h265");
        Log.d(TAG, "HevcToMP4 start: " + h265FilePath + " -> " + videoDir + " (" + str + ")");

        // 文件读取 + NAL 拆分 + 封装是重 IO/CPU 操作，放到后台线程，避免阻塞 UI
        mConverting = true;
        new Thread(() -> {
            VideoSaver saver = null;
            boolean ok = false;
            try {
                // 尺寸与素材 codec1.h265（704x576）一致；框架会以 hvcC/SPS 里的真实尺寸为准
                VideoConfig videoConfig = new VideoConfig(VideoConfig.MIME_H265, 704, 576);
                saver = new VideoSaver(videoDir, videoConfig, "mm");
                ok = convertHevcToMp4(h265FilePath, saver);
            } catch (Exception e) {
                Log.e(TAG, "HevcToMP4 error: " + e.getMessage());
            } finally {
                if (saver != null) {
                    saver.stop();
                }
                mConverting = false;
                final boolean result = ok;
                requireActivity().runOnUiThread(() ->
                        ToastManager.getInstance(getContext())
                                .showToast(result ? "HEVC转MP4完成" : "HEVC转MP4失败", 0));
            }
        }, "HevcToMp4").start();
    }

    /**
     * 流式读取 AnnexB 码流，按起始码切成完整 NAL 后逐条写入 VideoSaver。
     * 旧实现把 10KB 定长碎片（且误传未截断的临时 buffer）直接喂给"单 NAL"接口，
     * CsdFinder 永远找不到 csd，产出 0 字节文件。
     */
    private boolean convertHevcToMp4(String filePath, VideoSaver saver) throws IOException {
        ByteArrayOutputStream pending = new ByteArrayOutputStream();
        byte[] buf = new byte[128 * 1024];
        long[] ptsUs = {0};
        int[] nalCount = {0};
        boolean[] failed = {false};
        VideoSaver.NalSink sink = (nal, type, keyFrame) -> {
            if (!saver.writeFrame(nal, ptsUs[0], keyFrame)) {
                failed[0] = true;
            }
            if (type >= 0 && type <= 31) {   // 只有图像 NAL 推进时间戳
                ptsUs[0] += FRAME_STEP_US;
            }
            nalCount[0]++;
        };
        try (FileInputStream fis = new FileInputStream(filePath)) {
            int n;
            while ((n = fis.read(buf)) != -1) {
                pending.write(buf, 0, n);
                drainNals(pending, sink, false);
                if (failed[0]) {
                    return false;
                }
            }
        }
        drainNals(pending, sink, true);      // 文件结束，最后一段按完整 NAL 处理
        Log.d(TAG, "convertHevcToMp4 done, nal count: " + nalCount[0]);
        return !failed[0] && nalCount[0] > 0;
    }

    /** 从累积 buffer 中取出所有完整 NAL；不完整部分留在 pending 中等下一块数据 */
    private void drainNals(ByteArrayOutputStream pending, VideoSaver.NalSink sink, boolean flushTail) {
        byte[] all = pending.toByteArray();
        int first = nextStartCode(all, 0);
        if (first < 0) {
            pending.reset();
            return;
        }
        int i = first;
        int next;
        while ((next = nextStartCode(all, i + 3)) >= 0) {
            byte[] nal = Arrays.copyOfRange(all, i, next);
            sink.onNal(nal, hevcNalType(nal), isKeyNal(hevcNalType(nal)));
            i = next;
        }
        pending.reset();
        if (flushTail) {
            if (all.length - i >= 4) {
                byte[] nal = Arrays.copyOfRange(all, i, all.length);
                sink.onNal(nal, hevcNalType(nal), isKeyNal(hevcNalType(nal)));
            }
        } else {
            pending.write(all, i, all.length - i);
        }
    }

    private static int nextStartCode(byte[] data, int from) {
        for (int i = Math.max(from, 0); i + 2 < data.length; i++) {
            if (data[i] == 0 && data[i + 1] == 0 && data[i + 2] == 1) {
                return i;
            }
        }
        return -1;
    }

    private static int hevcNalType(byte[] nal) {
        int idx;
        if (nal.length >= 4 && nal[0] == 0 && nal[1] == 0 && nal[2] == 0 && nal[3] == 1) {
            idx = 4;
        } else if (nal.length >= 3 && nal[0] == 0 && nal[1] == 0 && nal[2] == 1) {
            idx = 3;
        } else {
            return -1;
        }
        return (nal[idx] & 0x7E) >> 1;
    }

    /** HEVC IRAP（16~21）算关键帧 */
    private static boolean isKeyNal(int type) {
        return type >= 16 && type <= 21;
    }



}
