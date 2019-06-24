package com.example.zhangzd.zzdpush.meida;


import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;

import com.example.zhangzd.zzdpush.LivePusher;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;


public class AudioChannel {
    private LivePusher mLivePusher;
    private int channels = 2;
    private int channelConfig;
    private AudioRecord audioRecord;
    private ExecutorService executorService;
    private Boolean isLiving = false;
    private int inputSamples; //缓冲区大小
    static final int sampleRateInHz = 44100;   //采样频率

    public AudioChannel(LivePusher livePusher) {
        mLivePusher = livePusher;
        executorService = Executors.newSingleThreadExecutor();
        if (channels == 2) {
            channelConfig =AudioFormat.CHANNEL_IN_STEREO;
        }else {
            channelConfig = AudioFormat.CHANNEL_IN_MONO;
        }

        mLivePusher.native_setaudioEncInfo(sampleRateInHz,channels);


        int buffSize = AudioRecord.getMinBufferSize(sampleRateInHz,channelConfig,AudioFormat.ENCODING_PCM_16BIT) * 2;
        inputSamples = (int) mLivePusher.getInputSamples() * 2;
        audioRecord = new AudioRecord(
                MediaRecorder.AudioSource.MIC,
                sampleRateInHz,channelConfig,
                AudioFormat.ENCODING_PCM_16BIT,
                buffSize > inputSamples? inputSamples:buffSize);
    }

    public void startLive() {
        isLiving = true;
        executorService.submit(new Runnable() {
            @Override
            public void run() {
                //开始获取音频流
                audioRecord.startRecording();
                while (isLiving) {
                    byte[] audioData = new byte[inputSamples];
                    audioRecord.read(audioData,0,audioData.length);
                    mLivePusher.native_pushAudio(audioData);
                }
            }
        });
    }
}
