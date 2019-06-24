#include <jni.h>
#include <string>
#include <pthread.h>
#include "x264.h"
#include "librtmp/rtmp.h"
#include "VideoChannel.h"
#include "macro.h"
#include "SafeQueue.h"
#include "AudioChannel.h"


VideoChannel *videoChannel;
AudioChannel *audioChannel;

int isStart = 0;
//线程索引
pthread_t pid;
uint32_t start_time;
//队列
SafeQueue<RTMPPacket *> packets;
int readyPushing = 0;


void releasePackets(RTMPPacket *pPacket);

//回调获取rtmpdump编码完成后的packet数据包
void callback(RTMPPacket *packet) {
    if (packet) {
        packet->m_nTimeStamp = RTMP_GetTime() - start_time;
        packets.put(packet);
    }
}

// 线程开启后的回调函数
void *threadStart(void *args) {
    char *url = static_cast<char *>(args);
    RTMP *rtmp = 0;
    rtmp = RTMP_Alloc();
    if (!rtmp) {
        LOGE("alloc rtmp失败");
        return NULL;
    }
    RTMP_Init(rtmp);
    int ret = RTMP_SetupURL(rtmp, url);
    if (!ret) {
        LOGE("设置地址失败:%s", url);
        return NULL;
    }

    rtmp->Link.timeout = 5;
    RTMP_EnableWrite(rtmp);
    ret = RTMP_Connect(rtmp, 0);
    if (!ret) {
        LOGE("连接服务器:%s", url);
        return NULL;
    }

    ret = RTMP_ConnectStream(rtmp, 0);
    if (!ret) {
        LOGE("连接流:%s", url);
        return NULL;
    }
    start_time= RTMP_GetTime();
    //表示可以开始推流了
    readyPushing = 1;
    packets.setWork(1);
    RTMPPacket *packet = 0;
    callback(audioChannel->getAudioTag());
    while (readyPushing) {
//        队列取数据  pakets
        packets.get(packet);
        LOGE("取出一帧数据");
        if (!readyPushing) {
            break;
        }
        if (!packet) {
            continue;
        }
        packet->m_nInfoField2 = rtmp->m_stream_id;
        ret = RTMP_SendPacket(rtmp, packet, 1);
        releasePackets(packet);
        LOGE("=============上传后%d",packets.size());
//        packet 释放
    }

    isStart = 0;
    readyPushing = 0;
    packets.setWork(0);
    packets.clear();
    if (rtmp) {
        RTMP_Close(rtmp);
        RTMP_Free(rtmp);
    }
    delete (url);
    return  0;
}

void releasePackets(RTMPPacket *pPacket) {
    if (pPacket) {
        RTMPPacket_Free(pPacket);
        delete pPacket;
        pPacket = 0;
    }

}

//初始化videoChannel
extern "C" JNIEXPORT void JNICALL
Java_com_example_zhangzd_zzdpush_LivePusher_native_1init(JNIEnv *env, jobject instance) {
    videoChannel = new VideoChannel;
    videoChannel->setCallback(callback);
    audioChannel = new AudioChannel;
    audioChannel->setAudioCallback(callback);
}


extern "C" JNIEXPORT void JNICALL
Java_com_example_zhangzd_zzdpush_LivePusher_native_1setVideoEncInfo(JNIEnv *env, jobject instance,
                                                                    jint width, jint height,
                                                                    jint fps, jint bitrate) {
    if (!videoChannel) {
        return;
    }
    videoChannel->setVideoEncInfo(width, height, fps, bitrate);

}


extern "C" JNIEXPORT void JNICALL
Java_com_example_zhangzd_zzdpush_LivePusher_native_1start(JNIEnv *env, jobject instance,
                                                          jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);

    //判断如果已经开启直播，则直接
    if (isStart) {
        return;
    }
    isStart = 1; //设置标记位为开启直播

    //开线程，从队列中取出数据，通过rtmpdump发送数据


    //将path保存，防止线程开启后path被销毁
    char *url = new char[strlen(path) + 1];  //加一个结束标识位\0
    strcpy(url, path);

    pthread_create(&pid, 0, threadStart, reinterpret_cast<void *>(url));

    env->ReleaseStringUTFChars(path_, path);
}


extern "C" JNIEXPORT void JNICALL
Java_com_example_zhangzd_zzdpush_LivePusher_native_1pushVideo(JNIEnv *env, jobject instance,
                                                              jbyteArray data_) {
    jbyte *data = env->GetByteArrayElements(data_, NULL);
    if (!videoChannel || !readyPushing) {
        return;
    }
    videoChannel->encodeData(data);


    env->ReleaseByteArrayElements(data_, data, 0);
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_zhangzd_zzdpush_LivePusher_stopLive(JNIEnv *env, jobject instance) {
    isStart = 0;
}


/**
 * 推送音频流
 */
extern "C" JNIEXPORT void JNICALL
Java_com_example_zhangzd_zzdpush_LivePusher_native_1pushAudio(JNIEnv *env, jobject instance,
                                                              jbyteArray data_) {
    jbyte *data = env->GetByteArrayElements(data_, NULL);


    if (audioChannel && readyPushing) {
        audioChannel->encodeData((data));
    }


    env->ReleaseByteArrayElements(data_, data, 0);
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_zhangzd_zzdpush_LivePusher_native_1setaudioEncInfo(JNIEnv *env, jobject instance,
                                                                    jint sampleRateInHz,
                                                                    jint channels) {
    if (audioChannel) {
        audioChannel->setAudioInfo(sampleRateInHz, channels);
    }


}
extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_zhangzd_zzdpush_LivePusher_native_1getInputSamples(JNIEnv *env, jobject instance) {

    if(audioChannel) {
        return audioChannel->getInputSamples();
    }
    return -1;

}