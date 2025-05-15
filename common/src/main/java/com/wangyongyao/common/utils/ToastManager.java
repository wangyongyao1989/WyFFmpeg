package com.wangyongyao.common.utils;

import android.content.Context;
import android.os.Handler;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.wangyongyao.common.R;

/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2025/5/15
 * Descibe : MyyFFmpeg com.wangyongyao.common.utils
 */
public class ToastManager {
    // 单例实例
    private static ToastManager instance;
    // Toast 对象
    private Toast toast;
    private Context mContext;
    // Handler 用于处理取消 Toast
    private static Handler handler = new Handler();
    // Runnable 用于取消 Toast 的任务
    private static Runnable cancelRunnable = new Runnable() {
        @Override
        public void run() {
            if (instance != null && instance.toast != null) {
                instance.toast.cancel();
            }
        }
    };

    private ToastManager(Context context) {
        // 获取应用上下文，防止内存泄漏
        mContext = context;
        // 初始化 Toast 对象
        this.toast = new Toast(context);
    }

    public static synchronized ToastManager getInstance(Context context) {
        if (instance == null) {
            instance = new ToastManager(context);
        }
        return instance;
    }

    /**
     * 在屏幕下部显示Toast提示信息.
     *
     * @param msg      - 提示消息
     * @param lastTime - 持续时间，0-短时间，LENGTH_SHORT；1-长时间，LENGTH_LONG；
     */
    public void showToast(String msg, int lastTime) {

        handler.removeCallbacks(cancelRunnable);
        View view = View.inflate(mContext, R.layout.common_toast, null);
        TextView tvMsg = view.findViewById(R.id.common_toast_tv);
        tvMsg.setText(msg);

        toast.setView(view);
        toast.setDuration(lastTime);

        toast.show();
    }

    /**
     * 解决Toast重复显示，显示不及时的问题.
     *
     * @param msg      - 提示消息
     * @param lastTime - 持续时间，0-短时间，LENGTH_SHORT；1-长时间，LENGTH_LONG；
     */
    public void refreshToast(String msg, int lastTime) {
        handler.removeCallbacks(cancelRunnable);
        View view = View.inflate(mContext, R.layout.common_toast, null);
        TextView tvMsg = view.findViewById(R.id.common_toast_tv);
        tvMsg.setText(msg);

        toast.setView(view);
        toast.setDuration(lastTime);

        toast.show();

        handler.postDelayed(cancelRunnable, lastTime == Toast.LENGTH_LONG ? 3500 : 2000);

    }
}
