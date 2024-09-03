package com.wangyongyao.glplay.camerahelper.camerahelper;

import static androidx.core.content.PermissionChecker.checkSelfPermission;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.ImageReader;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.util.Size;
import android.util.SparseIntArray;
import android.view.Surface;

import androidx.annotation.NonNull;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;

/**
 * author : wangyongyao https://github.com/wangyongyao1989
 * Create Time : 2024/9/3 10:57
 * Descibe : MyyFFmpeg com.example.myyffmpeg.utils
 */
public class CameraDataHelper implements PreviewFrameHandler {
    private static final String TAG = CameraDataHelper.class.toString();
    private static final int IMAGE_BUFFER_SIZE = 3;

    private static final SparseIntArray ORIENTATIONS = new SparseIntArray();

    static {
        ORIENTATIONS.append(Surface.ROTATION_0, 90);
        ORIENTATIONS.append(Surface.ROTATION_90, 0);
        ORIENTATIONS.append(Surface.ROTATION_180, 270);
        ORIENTATIONS.append(Surface.ROTATION_270, 180);
    }

    private final Context mContext;
    private final VideoCapture mVideoCapture;
    private final Semaphore mCameraOpenCloseLock = new Semaphore(1);
    private CameraCaptureSession mCaptureSession;
    private CameraDevice mCameraDevice;
    private String mCameraId;
    private Handler mBackgroundHandler;
    private HandlerThread mBackgroundThread;
    private ImageReader mImageReader;
    private Integer mSensorOrientation;
    private List<Size> mOutputSizes = new ArrayList<>();
    private Size mPreviewSize;
    private CameraDataListener mCameraDataListener;

    public CameraDataHelper(Context context, CameraDataListener cameraDataListener) {
        mContext = context;
        mCameraDataListener = cameraDataListener;
        mVideoCapture = new VideoCapture(this);
    }

    @Override
    public void onPreviewFrame(byte[] data, int width, int height) {
        if (mCameraDataListener != null) {
            mCameraDataListener.onPreviewFrame(data, width, height);
        }
    }

    public Integer getSensorOrientation() {
        return mSensorOrientation;
    }

    public List<Size> getOutputSizes() {
        return mOutputSizes;
    }

    public void initialize(int width, int height) {
        mPreviewSize = getOptimalPreviewSize(width, height);

        openCamera();
    }

    public void destroy() {

    }

    public void startCamera() {
        startBackgroundThread();
    }

    public void stopCamera() {
        closeCamera();
        stopBackgroundThread();
    }

    public void changeSize(Size size) {
        mPreviewSize = size;

        stopCamera();
        openCamera();
        startCamera();
    }

    /**
     * Opens the camera.
     */
    @SuppressLint({"WrongConstant", "MissingPermission"})
    public void openCamera() {
        if (checkSelfPermission(mContext, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
            return;
        }

        mImageReader = ImageReader.newInstance(mPreviewSize.getWidth(), mPreviewSize.getHeight(), ImageFormat.YUV_420_888, IMAGE_BUFFER_SIZE);
        mImageReader.setOnImageAvailableListener(mVideoCapture, mBackgroundHandler);

        Log.i(TAG, "openCamera");

        CameraManager manager = (CameraManager) mContext.getSystemService(Context.CAMERA_SERVICE);
        try {
            if (!mCameraOpenCloseLock.tryAcquire(2500, TimeUnit.MILLISECONDS)) {
                throw new RuntimeException("Time out waiting to lock camera opening.");
            }
            manager.openCamera(mCameraId, mStateCallback, mBackgroundHandler);
        } catch (CameraAccessException e) {
            Log.e(TAG, "Cannot access the camera " + e);
        } catch (InterruptedException e) {
            throw new RuntimeException("Interrupted while trying to lock camera opening.", e);
        }
    }

    /**
     * Closes the current {@link CameraDevice}.
     */
    public void closeCamera() {
        try {
            mCameraOpenCloseLock.acquire();
            if (null != mCaptureSession) {
                mCaptureSession.close();
                mCaptureSession = null;
            }
            if (null != mCameraDevice) {
                mCameraDevice.close();
                mCameraDevice = null;
            }
            if (null != mImageReader) {
                mImageReader.close();
                mImageReader = null;
            }
        } catch (InterruptedException e) {
            throw new RuntimeException("Interrupted while trying to lock camera closing.", e);
        } finally {
            mCameraOpenCloseLock.release();
        }

        Log.i(TAG, "closeCamera");
    }

    /**
     * Starts a background thread and its {@link Handler}.
     */
    public void startBackgroundThread() {
        mBackgroundThread = new HandlerThread("CameraBackground");
        mBackgroundThread.start();
        mBackgroundHandler = new Handler(mBackgroundThread.getLooper());
    }

    /**
     * Stops the background thread and its {@link Handler}.
     */
    public void stopBackgroundThread() {
        mBackgroundThread.quitSafely();
        try {
            mBackgroundThread.join();
            mBackgroundThread = null;
            mBackgroundHandler = null;
        } catch (InterruptedException e) {
            Log.e(TAG, "stopBackgroundThread " + e);
        }
    }

    private CaptureRequest createCaptureRequest() {
        if (null == mCameraDevice) return null;
        try {
            CaptureRequest.Builder builder = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_RECORD);
            builder.addTarget(mImageReader.getSurface());
            return builder.build();
        } catch (CameraAccessException e) {
            Log.e(TAG, "createCaptureRequest " + e);
            return null;
        }
    }

    /**
     * {@link CameraDevice.StateCallback} is called when {@link CameraDevice} changes its state.
     */
    private final CameraDevice.StateCallback mStateCallback = new CameraDevice.StateCallback() {

        @Override
        public void onOpened(@NonNull CameraDevice cameraDevice) {
            // This method is called when the camera is opened.  We start camera preview here.
            mCameraOpenCloseLock.release();
            mCameraDevice = cameraDevice;
            createCaptureSession();
        }

        @Override
        public void onDisconnected(@NonNull CameraDevice cameraDevice) {
            mCameraOpenCloseLock.release();
            cameraDevice.close();
            mCameraDevice = null;
        }

        @Override
        public void onError(@NonNull CameraDevice cameraDevice, int error) {
            mCameraOpenCloseLock.release();
            cameraDevice.close();
            mCameraDevice = null;
        }
    };

    private void createCaptureSession() {
        try {
            if (null == mCameraDevice || null == mImageReader) return;
            mCameraDevice.createCaptureSession(Collections.singletonList(mImageReader.getSurface()),
                    sessionStateCallback, mBackgroundHandler);

        } catch (CameraAccessException e) {
            Log.e(TAG, "createCaptureSession " + e);
        }
    }

    /**
     * Creates a new {@link CameraCaptureSession} for camera preview.
     */
    private final CameraCaptureSession.StateCallback sessionStateCallback = new CameraCaptureSession.StateCallback() {
        @Override
        public void onConfigured(@NonNull CameraCaptureSession session) {
            mCaptureSession = session;
            try {
                CaptureRequest captureRequest = createCaptureRequest();
                if (captureRequest != null) {
                    session.setRepeatingRequest(captureRequest, null, mBackgroundHandler);
                } else {
                    Log.e(TAG, "captureRequest is null");
                }
            } catch (CameraAccessException e) {
                Log.e(TAG, "onConfigured " + e);
            }
        }

        @Override
        public void onConfigureFailed(@NonNull CameraCaptureSession session) {
            Log.e(TAG, "onConfigureFailed");
        }
    };

    public Size getOptimalPreviewSize(int w, int h) {
        CameraManager manager = (CameraManager) mContext.getSystemService(Context.CAMERA_SERVICE);
        try {
            for (String cameraId : manager.getCameraIdList()) {
                CameraCharacteristics characteristics = manager.getCameraCharacteristics(cameraId);

                // We don't use a front facing camera in this sample.
                Integer facing = characteristics.get(CameraCharacteristics.LENS_FACING);
                if (facing != null && facing == CameraCharacteristics.LENS_FACING_FRONT) {
                    continue;
                }
                StreamConfigurationMap streamConfigs = characteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
                if (streamConfigs != null) {
                    mOutputSizes = Arrays.asList(streamConfigs.getOutputSizes(SurfaceTexture.class));
                }
                mSensorOrientation = characteristics.get(CameraCharacteristics.SENSOR_ORIENTATION);
                mCameraId = cameraId;
            }
        } catch (CameraAccessException e) {
            Log.e(TAG, "Cannot access the camera." + e.toString());
        }

        // Use a very small tolerance because we want an exact match.
        final double ASPECT_TOLERANCE = 0.1;
        double targetRatio = (double) w / h;
        if (mOutputSizes == null)
            return null;

        Size optimalSize = null;

        // Start with max value and refine as we iterate over available preview sizes. This is the
        // minimum difference between view and camera height.
        double minDiff = Double.MAX_VALUE;

        // Try to find a preview size that matches aspect ratio and the target view size.
        // Iterate over all available sizes and pick the largest size that can fit in the view and
        // still maintain the aspect ratio.
        for (Size size : mOutputSizes) {
            double ratio = (double) size.getWidth() / size.getHeight();
            if (Math.abs(ratio - targetRatio) > ASPECT_TOLERANCE)
                continue;
            if (Math.abs(size.getHeight() - h) < minDiff) {
                optimalSize = size;
                minDiff = Math.abs(size.getHeight() - h);
            }
        }

        // Cannot find preview size that matches the aspect ratio, ignore the requirement
        if (optimalSize == null) {
            for (Size size : mOutputSizes) {
                if (Math.abs(size.getHeight() - h) < minDiff) {
                    optimalSize = size;
                    minDiff = Math.abs(size.getHeight() - h);
                }
            }
        }

        return optimalSize;
    }

    private int getOrientation() {
        int rotation = ((Activity) mContext).getWindowManager().getDefaultDisplay().getRotation();
        return (ORIENTATIONS.get(rotation) + getSensorOrientation() + 270) % 360;
    }
}
