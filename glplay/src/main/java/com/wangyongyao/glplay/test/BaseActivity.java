package com.wangyongyao.glplay.test;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Size;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.fragment.app.FragmentActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.wangyongyao.glplay.R;

import java.util.ArrayList;
import java.util.List;

public abstract class BaseActivity extends FragmentActivity
        implements SimpleGestureFilter.SimpleGestureListener {

    protected CameraController mCameraController;
    protected SimpleGestureFilter mDetector;
    protected ResolutionDialog mResolutionDialog;
    protected int mParams;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        DisplayMetrics displayMetrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);

        mDetector = new SimpleGestureFilter(this, this);
        mResolutionDialog = new ResolutionDialog(this);
    }

    protected void setup(SurfaceView surfaceView) {
        surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder holder) {

            }

            @Override
            public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
                mCameraController.initialize(width, height);
            }

            @Override
            public void surfaceCreated(@NonNull SurfaceHolder holder) {

            }
        });
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        mDetector.onTouchEvent(event);
        return super.onTouchEvent(event);
    }

    public void showResolutionDialog(List<Size> items) {
        mResolutionDialog.setItems(items);
        mResolutionDialog.show();
    }

    class BaseDialog extends Dialog {
        RecyclerView mRecyclerView;
        TextView mTextView;

        BaseDialog(@NonNull Context context) {
            super(context);

            setContentView(R.layout.dialog);

            if (getWindow() != null) {
                getWindow().setBackgroundDrawableResource(android.R.color.transparent);
            }
            mRecyclerView = findViewById(R.id.recycler_view);
            mRecyclerView.setHasFixedSize(true);
            mRecyclerView.setLayoutManager(new LinearLayoutManager(BaseActivity.this));

            mTextView = findViewById(R.id.text_view);
        }
    }

    public class ResolutionDialog extends BaseDialog {
        private final ItemAdapter<Size> mAdapter;

        ResolutionDialog(@NonNull Context context) {
            super(context);

            mTextView.setText(R.string.select_resolution);
            ArrayList<Size> items = new ArrayList<>();
            ItemAdapter.ItemListener<Size> mListener = item -> {
                dismiss();
                mCameraController.changeSize(item);
            };
            mAdapter = new ItemAdapter<>(items, mListener, R.layout.size_list_item);
            mRecyclerView.setAdapter(mAdapter);
        }

        void setItems(List<Size> items) {
            mAdapter.setItems(items);
        }
    }
}
