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
        //获取到数据NV21 --> i420 ，编码放入队列中，以便推流可以获取到

        if (isLiving) {
            livePusher.native_pushVideo(data);
        }

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

    public void startLive() {
        isLiving = true;   //开始直播
    }
}
