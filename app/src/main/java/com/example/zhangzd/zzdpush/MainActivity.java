package com.example.zhangzd.zzdpush;

import android.Manifest;
import android.content.pm.PackageManager;
import android.hardware.Camera;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Toast;


public class MainActivity extends AppCompatActivity {

    private LivePusher livePusher;
    // 要申请的权限
    private String[] permissions = {
                    Manifest.permission.INTERNET,
                    Manifest.permission.CAMERA,
                    Manifest.permission.RECORD_AUDIO};
    SurfaceView surfaceView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        surfaceView = findViewById(R.id.surfaceView);
        livePusher = new LivePusher(this, 800, 400, 800_000, 10, Camera.CameraInfo.CAMERA_FACING_BACK);

        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (ContextCompat.checkSelfPermission(this,permissions[0]) == PackageManager.PERMISSION_DENIED) {
                ActivityCompat.requestPermissions(this,permissions,0);
            }else {
                livePusher.setPreviewDisplay(surfaceView.getHolder());
            }
        }else {
            //  设置摄像头预览的界面
            livePusher.setPreviewDisplay(surfaceView.getHolder());
        }
    }



    public void switchCamera(View view) {
        livePusher.switchCamera();
    }

    public void startLive(View view) {
        livePusher.startLive("z");
    }

    public void stopLive(View view) {
        livePusher.stopLive();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {

        if (requestCode == 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
            livePusher.setPreviewDisplay(surfaceView.getHolder());
        }else {
            Toast.makeText(this,"获取权限失败，无法打开摄像头",Toast.LENGTH_SHORT).show();
        }


        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
    }
}
