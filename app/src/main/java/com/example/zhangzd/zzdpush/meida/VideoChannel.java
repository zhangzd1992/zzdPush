package com.example.zhangzd.zzdpush.meida;

import android.app.Activity;
import android.hardware.Camera;
import android.view.SurfaceHolder;

import com.example.zhangzd.zzdpush.LivePusher;


public class VideoChannel implements Camera.PreviewCallback, CameraHelper.OnChangedSizeListener {
    private CameraHelper cameraHelper;
    private int mBitrate;
    private int mFps;
    private boolean isLiving;
    LivePusher livePusher;
    public VideoChannel(LivePusher livePusher, Activity activity, int width, int height, int bitrate, int fps, int cameraId) {
        mBitrate = bitrate;
        this.livePusher = livePusher;
        mFps = fps;
        cameraHelper = new CameraHelper(activity, cameraId, width, height);
        cameraHelper.setPreviewCallback(this);
        cameraHelper.setOnChangedSizeListener(this);
    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {

    }


    @Override
    public void onChanged(int w, int h) {
        livePusher.native_setVideoEncInfo(w,h,mFps,mBitrate);
    }
    public void switchCamera() {
        cameraHelper.switchCamera();
    }

    public void setPreviewDisplay(SurfaceHolder surfaceHolder) {
        cameraHelper.setPreviewDisplay(surfaceHolder);
    }
}
