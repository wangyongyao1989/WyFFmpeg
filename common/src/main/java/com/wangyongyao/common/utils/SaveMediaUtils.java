package com.wangyongyao.common.utils;

import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.media.MediaScannerConnection;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.provider.MediaStore;
import android.util.Log;

import androidx.annotation.RequiresApi;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.nio.file.Files;
import java.util.Objects;

public class SaveMediaUtils {
    private static final String TAG = SaveMediaUtils.class.getSimpleName();

    /**
     * 将图片文件保存到系统相册
     */
    public static boolean saveImgFileToAlbum(Context context, String imageFilePath) {
//        Log.d(TAG, "saveImgToAlbum() imageFile = [" + imageFilePath + "]");
        try {
            Bitmap bitmap = BitmapFactory.decodeFile(imageFilePath);
            return saveBitmapToAlbum(context, bitmap);
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
    }

    /**
     * 将bitmap保存到系统相册
     */
    public static boolean saveBitmapToAlbum(Context context, Bitmap bitmap) {
        if (bitmap == null) return false;
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.Q) {
            return saveBitmapToAlbumBeforeQ(context, bitmap);
        } else {
            return saveBitmapToAlbumAfterQ(context, bitmap);
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.Q)
    private static boolean saveBitmapToAlbumAfterQ(Context context, Bitmap bitmap) {
        Uri contentUri;
        if (Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)) {
            contentUri = MediaStore.Images.Media.EXTERNAL_CONTENT_URI;
        } else {
            contentUri = MediaStore.Images.Media.INTERNAL_CONTENT_URI;
        }
        ContentValues contentValues = getImageContentValues(context);
        Uri uri = context.getContentResolver().insert(contentUri, contentValues);
        if (uri == null) {
            return false;
        }
        OutputStream os = null;
        try {
            os = context.getContentResolver().openOutputStream(uri);
            bitmap.compress(Bitmap.CompressFormat.JPEG, 50, os);
//            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
//                Files.copy(bitmapFile.toPath(), os);
//            }
            contentValues.clear();
            contentValues.put(MediaStore.MediaColumns.IS_PENDING, 0);
            context.getContentResolver().update(uri, contentValues, null, null);
            return true;
        } catch (Exception e) {
            context.getContentResolver().delete(uri, null, null);
            e.printStackTrace();
            return false;
        } finally {
            try {
                if (os != null) {
                    os.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    private static boolean saveBitmapToAlbumBeforeQ(Context context, Bitmap bitmap) {
        File picDir = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DCIM);
        File destFile = new File(picDir, context.getPackageName() + File.separator + System.currentTimeMillis() + ".jpg");
//            FileUtils.copy(imageFile, destFile.getAbsolutePath());
        OutputStream os = null;
        boolean result = false;
        try {
            if (!destFile.exists()) {
                Objects.requireNonNull(destFile.getParentFile()).mkdirs();
                destFile.createNewFile();
            }
            os = new BufferedOutputStream(new FileOutputStream(destFile));
            result = bitmap.compress(Bitmap.CompressFormat.JPEG, 50, os);
            if (!bitmap.isRecycled()) bitmap.recycle();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                if (os != null) {
                    os.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        MediaScannerConnection.scanFile(
                context,
                new String[]{destFile.getAbsolutePath()},
                new String[]{"image/*"},
                (path, uri) -> {
                    Log.d(TAG, "saveImgToAlbum: " + path + " " + uri);
                    // Scan Completed
                });
        return result;
    }

    /**
     * 获取图片的ContentValue
     *
     * @param context
     */
    @RequiresApi(api = Build.VERSION_CODES.Q)
    public static ContentValues getImageContentValues(Context context) {
        ContentValues contentValues = new ContentValues();
        contentValues.put(MediaStore.Images.Media.DISPLAY_NAME, System.currentTimeMillis() + ".jpg");
        contentValues.put(MediaStore.Images.Media.MIME_TYPE, "image/*");
        contentValues.put(MediaStore.Images.Media.RELATIVE_PATH, Environment.DIRECTORY_DCIM + File.separator + context.getPackageName());
        contentValues.put(MediaStore.MediaColumns.IS_PENDING, 1);
        contentValues.put(MediaStore.Images.Media.DATE_TAKEN, System.currentTimeMillis());
        contentValues.put(MediaStore.Images.Media.DATE_MODIFIED, System.currentTimeMillis());
        contentValues.put(MediaStore.Images.Media.DATE_ADDED, System.currentTimeMillis());
        return contentValues;
    }

    /**
     * 将视频保存到系统相册
     */
    public static boolean saveVideoToAlbum(Context context, String videoFile) {
//        Log.d(TAG, "saveVideoToAlbum() videoFile = [" + videoFile + "]");
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.Q) {
            return saveVideoToAlbumBeforeQ(context, videoFile);
        } else {
            return saveVideoToAlbumAfterQ(context, videoFile);
        }


    }

    @RequiresApi(api = Build.VERSION_CODES.Q)
    private static boolean saveVideoToAlbumAfterQ(Context context, String videoFile) {
        try {
            ContentResolver contentResolver = context.getContentResolver();
            File tempFile = new File(videoFile);
            ContentValues contentValues = getVideoContentValues(context, tempFile, System.currentTimeMillis());
            Uri uri = contentResolver.insert(MediaStore.Video.Media.EXTERNAL_CONTENT_URI, contentValues);
            copyFileAfterQ(context, contentResolver, tempFile, uri);
            contentValues.clear();
            contentValues.put(MediaStore.MediaColumns.IS_PENDING, 0);
            context.getContentResolver().update(uri, contentValues, null, null);
            context.sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, uri));
            return true;
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
    }

    private static boolean saveVideoToAlbumBeforeQ(Context context, String videoFile) {
        File picDir = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DCIM);
        File tempFile = new File(videoFile);
        File destFile = new File(picDir, context.getPackageName() + File.separator + tempFile.getName());
        FileInputStream ins = null;
        BufferedOutputStream ous = null;
        try {
            ins = new FileInputStream(tempFile);
            ous = new BufferedOutputStream(new FileOutputStream(destFile));
            long nread = 0L;
            byte[] buf = new byte[1024];
            int n;
            while ((n = ins.read(buf)) > 0) {
                ous.write(buf, 0, n);
                nread += n;
            }
            MediaScannerConnection.scanFile(
                    context,
                    new String[]{destFile.getAbsolutePath()},
                    new String[]{"video/*"},
                    (path, uri) -> {
                        Log.d(TAG, "saveVideoToAlbum: " + path + " " + uri);
                        // Scan Completed
                    });
            return true;
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        } finally {
            try {
                if (ins != null) {
                    ins.close();
                }
                if (ous != null) {
                    ous.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    private static void copyFileAfterQ(Context context, ContentResolver localContentResolver
            , File tempFile, Uri localUri) throws IOException {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q &&
                context.getApplicationInfo().targetSdkVersion >= Build.VERSION_CODES.Q) {
            //拷贝文件到相册的uri,android10及以上得这么干，否则不会显示。可以参考ScreenMediaRecorder的save方法
            OutputStream os = localContentResolver.openOutputStream(localUri);
            Files.copy(tempFile.toPath(), os);
            os.close();
            tempFile.delete();
        }
    }


    /**
     * 获取视频的contentValue
     */
    @RequiresApi(api = Build.VERSION_CODES.Q)
    public static ContentValues getVideoContentValues(Context context, File paramFile, long timestamp) {
        ContentValues localContentValues = new ContentValues();
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            localContentValues.put(MediaStore.Video.Media.RELATIVE_PATH, Environment.DIRECTORY_DCIM
                    + File.separator + context.getPackageName());
        }
        localContentValues.put(MediaStore.Video.Media.TITLE, paramFile.getName());
        localContentValues.put(MediaStore.Video.Media.DISPLAY_NAME, paramFile.getName());
        localContentValues.put(MediaStore.Video.Media.MIME_TYPE, "video/mp4");
        localContentValues.put(MediaStore.Video.Media.DATE_TAKEN, timestamp);
        localContentValues.put(MediaStore.Video.Media.DATE_MODIFIED, timestamp);
        localContentValues.put(MediaStore.Video.Media.DATE_ADDED, timestamp);
        localContentValues.put(MediaStore.Video.Media.SIZE, paramFile.length());
        return localContentValues;
    }

}