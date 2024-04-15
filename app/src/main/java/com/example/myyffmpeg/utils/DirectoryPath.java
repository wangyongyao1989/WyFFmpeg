package com.example.myyffmpeg.utils;

import android.annotation.SuppressLint;
import android.content.Context;
import android.os.Build;
import android.os.Environment;
import android.os.storage.StorageManager;
import android.util.Log;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.InputStream;
import java.lang.reflect.Array;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;

public class DirectoryPath {

    private static final String TAG = DirectoryPath.class.getSimpleName();
    private static final String MM_DIR = "WY";
    private static final String MM_VIDEO = "Video";
    private static final String MM_PHOTO = "Photo";
    private static final String MM_CARSHLOG = "CarshLog";
    private static final String MM_DATA = "data";


    public static String createPhotoDir(Context context) {
        String filesDirString = getPicDirString(context);
        String PhotoStringBuilder = filesDirString
                + File.separator;
        return PhotoStringBuilder;
    }

    public static String createVideoDir(Context context) {
        String filesDirString = getVideoDirString(context);
        String videoString = filesDirString
                + File.separator;
        return videoString;
    }

    public static String createCarshLogDir(Context context) {
        String filesDirString = getCarshLogDirString(context);
        String PhotoStringBuilder = filesDirString +
                File.separator;
        return PhotoStringBuilder;
    }

    public static String createSDCardDataDir(Context context) {
        String filesDirString = getMMDataString(context);
        String PhotoStringBuilder = filesDirString +
                File.separator;
        return PhotoStringBuilder;
    }

    @SuppressLint("ObsoleteSdkInt")
    private static String getPicDirString(Context context) {
        File filePath;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.GINGERBREAD_MR1) {
            File externalFilesDir = context.getExternalFilesDir(null);
            String path = externalFilesDir.getAbsolutePath()
                    + File.separator
                    + MM_DIR
                    + File.separator
                    + MM_PHOTO
                    + File.separator;
            filePath = new File(path);
            if (!filePath.exists()) {
                filePath.mkdirs();
            }
        } else {
            filePath = new File(createSDCardPhotoDir());
        }
        return filePath.toString();
    }

    @SuppressLint("ObsoleteSdkInt")
    private static String getVideoDirString(Context context) {
        File filePath;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.GINGERBREAD_MR1) {
            File externalFilesDir = context.getExternalFilesDir(null);
            String path = externalFilesDir.getAbsolutePath()
                    + File.separator
                    + MM_DIR
                    + File.separator
                    + MM_VIDEO
                    + File.separator;
            filePath = new File(path);
            if (!filePath.exists()) {
                filePath.mkdirs();
            }
        } else {
            filePath = new File(createSDCardVideoDir());
        }
        return filePath.toString();
    }

    @SuppressLint("ObsoleteSdkInt")
    private static String getCarshLogDirString(Context context) {
        File filePath;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.GINGERBREAD_MR1) {
            File externalFilesDir = context.getExternalFilesDir(null);
            String path = externalFilesDir.getAbsolutePath()
                    + File.separator
                    + MM_DIR
                    + File.separator
                    + MM_CARSHLOG
                    + File.separator;
            filePath = new File(path);
            if (!filePath.exists()) {
                filePath.mkdirs();
            }
        } else {
            filePath = new File(createSDCardCarshLogDir());
        }
        return filePath.toString();
    }

    @SuppressLint("ObsoleteSdkInt")
    private static String getMMDataString(Context context) {
        File filePath;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.GINGERBREAD_MR1) {
            File externalFilesDir = context.getExternalFilesDir(null);
            String path = externalFilesDir.getAbsolutePath()
                    + File.separator
                    + MM_DIR
                    + File.separator
                    + MM_DATA
                    + File.separator;
            filePath = new File(path);
            if (!filePath.exists()) {
                filePath.mkdirs();
            }
        } else {
            filePath = new File(createSDCardDataDir());
        }
        return filePath.toString();
    }

    private static String getMMDataPath() {
        String dataPath = Environment.getExternalStorageDirectory() +
                File.separator +
                MM_DIR +
                File.separator +
                MM_DATA +
                File.separator;
        return dataPath;
    }

    /**
     * CreateSDCardDir
     */
    private static String createSDCardPhotoDir() {
        String photoDir;
        if (isHaveSDCard()) {
            photoDir = File.separator
                    + MM_DIR
                    + File.separator
                    + MM_PHOTO
                    + File.separator;
        } else {
            photoDir = Environment.getExternalStorageDirectory()
                    + File.separator
                    + MM_DIR
                    + File.separator
                    + MM_PHOTO
                    + File.separator;
        }
        return photoDir;
    }

    private static String createSDCardVideoDir() {
        String videoDir;
        if (isHaveSDCard()) {
            videoDir = File.separator
                    + MM_DIR
                    + File.separator
                    + MM_VIDEO
                    + File.separator;
        } else {
            videoDir = Environment.getExternalStorageDirectory()
                    + File.separator
                    + MM_DIR
                    + File.separator
                    + MM_VIDEO
                    + File.separator;
        }

        return videoDir;
    }

    private static String createSDCardCarshLogDir() {
        String carshDir;
        if (isHaveSDCard()) {
            carshDir = File.separator
                    + MM_DIR
                    + File.separator
                    + MM_CARSHLOG
                    + File.separator;
        } else {
            carshDir = Environment.getExternalStorageDirectory()
                    + File.separator
                    + MM_DIR
                    + File.separator
                    + MM_CARSHLOG
                    + File.separator;
        }

        return carshDir;
    }

    private static String createSDCardDataDir() {
        String dataDir;
        if (isHaveSDCard()) {
            dataDir = File.separator
                    + MM_DIR
                    + File.separator
                    + MM_DATA
                    + File.separator;

        } else {
            dataDir = Environment.getExternalStorageDirectory()
                    + File.separator
                    + MM_DIR
                    + File.separator
                    + MM_DATA
                    + File.separator;
        }

        return dataDir;
    }

    public static ArrayList<File> getAllDataFile() {
        // 文件夹路径
        String collectionPath = getMMDataPath();
        ArrayList<File> fileList = new ArrayList<>();
//        Log.e(TAG, "collectionPath:" + collectionPath);
        File file = new File(collectionPath);
        File[] tempList = file.listFiles();
//        Log.e(TAG, "tempList:" + tempList);
        if (tempList == null || tempList.length < 1) return null;
        for (int i = 0; i < tempList.length; i++) {
            if (tempList[i].isFile()) {
                if (tempList[i].getName().endsWith(MM_DIR)) {
//                    Log.e(TAG, "fileName:" + tempList[i].getName());
                    fileList.add(tempList[i]);
                }
            }
        }
        return fileList;
    }

    public static void sortList(List<File> paramList) {
        paramList.sort((o1, o2) -> {
            float l = (o1.lastModified() - o2.lastModified());
            return l > 0 ? -1 : 1;
        });
    }


    public static byte[] readAssetsStream(Context context, String fileName) {
        try {

            InputStream inStream = context.getResources().getAssets().open(fileName);
            Log.e(TAG, "fileName:" + inStream.toString());
            ByteArrayOutputStream outStream = new ByteArrayOutputStream();
            byte[] buffer = new byte[1024];
            int len = -1;
            while ((len = inStream.read(buffer)) != -1) {
                Log.e(TAG, "buffer:" + buffer.toString());
                outStream.write(buffer, 0, len);
            }
            outStream.close();
            inStream.close();
            return outStream.toByteArray();
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }

    /**
     * 判断手机是否有SD卡。
     *
     * @return 有SD卡返回true，没有返回false。
     */
    private static boolean isHaveSDCard() {

        if (Environment.MEDIA_MOUNTED.equals(Environment.getExternalStorageState())) {
            return true;
        } else if (Environment.MEDIA_MOUNTED_READ_ONLY.equals(Environment.getExternalStorageState())) {
            return false;
        } else {
            return false;
        }
    }

    public static String getStoragePath(Context mContext, boolean is_removale) {
        StorageManager mStorageManager = (StorageManager) mContext.getSystemService(Context.STORAGE_SERVICE);
        Class storageVolumeClazz = null;
        try {
            storageVolumeClazz = Class.forName("android.os.storage.StorageVolume");
            Method getVolumeList = mStorageManager.getClass().getMethod("getVolumeList");
            Method getPath = storageVolumeClazz.getMethod("getPath");
            Method isRemovable = storageVolumeClazz.getMethod("isRemovable");
            Object result = getVolumeList.invoke(mStorageManager);
            final int length = Array.getLength(result);
            for (int i = 0; i < length; i++) {
                Object storageVolumeElement = Array.get(result, i);
                String path = (String) getPath.invoke(storageVolumeElement);
                boolean removable = (Boolean) isRemovable.invoke(storageVolumeElement);
                if (is_removale == removable) {
                    return path;
                }
            }
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        }
        return null;
    }


}
