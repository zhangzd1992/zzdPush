package com.example.zhangzd.zzdpush;

import android.app.Activity;
import android.view.SurfaceHolder;

import com.example.zhangzd.zzdpush.meida.AudioChannel;
import com.example.zhangzd.zzdpush.meida.VideoChannel;


public class LivePusher {
    private AudioChannel audioChannel;
    private VideoChannel videoChannel;
    static {
        System.loadLibrary("native-lib");
    }

    public LivePusher(Activity activity, int width, int height, int bitrate,int fps, int cameraId) {
        native_init();
        videoChannel = new VideoChannel(this, activity, width, height, bitrate, fps, cameraId);
        audioChannel = new AudioChannel(this);
    }
    public void setPreviewDisplay(SurfaceHolder surfaceHolder) {
        videoChannel.setPreviewDisplay(surfaceHolder);
    }
    public void switchCamera() {
        videoChannel.switchCamera();
    }

    /**
     * 开始直播，设置推流地址
     * @param path
     */
    public void startLive(String path) {
        native_start(path);
        videoChannel.startLive();
    }

    public native void native_init();

    /**
     * 设置视频编码参数
     * @param width    宽
     * @param height   高
     * @param fps      帧率
     * @param bitrate  码率
     */
    public native void native_setVideoEncInfo(int width, int height, int fps, int bitrate);
    public native void native_start(String path);

    /**
     * 推送数据流
     * @param data
     */
    public native void native_pushVideo(byte[] data);

    public native void stopLive();
}
