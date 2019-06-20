#include <jni.h>
#include <string>
#include <pthread.h>
#include "x264.h"
#include "librtmp/rtmp.h"
#include "VideoChannel.h"
#include "macro.h"
#include "SafeQueue.h"


VideoChannel *videoChannel;

int isStart = 0;
//线程索引
pthread_t pid;
uint32_t start_time;
//队列
SafeQueue<RTMPPacket *> packets;
int readyPushing = 0;


void releasePackets(RTMPPacket *pPacket);
//回调获取rtmpdump编码完成后的packet数据包
void videoCallback(RTMPPacket *packet) {
    if (packet) {
        packet->m_nTimeStamp = RTMP_GetTime() - start_time;
        packets.put(packet);
    }
}

// 线程开启后的回调函数
void* threadStart(void* args) {
    //初始化rtmpdump

    char *url = static_cast<char *>(args);
    RTMP * rtmp= 0;
    rtmp  = RTMP_Alloc();
    if (!rtmp) {
        LOGE("alloc rtmp失败");
        return NULL;
    }


    RTMP_Init(rtmp);
   int ret =  RTMP_SetupURL(rtmp,url);
    if (!ret) {
        LOGE("设置地址失败:%s", url);
        return NULL;
    }

    RTMP_EnableWrite(rtmp);
    rtmp->Link.timeout = 5;
    ret = RTMP_Connect(rtmp,0);
    if (!ret) {
        LOGE("连接服务器:%s", url);
        return NULL;
    }
    ret = RTMP_ConnectStream(rtmp,0);
    if (!ret) {
        LOGE("连接流:%s", url);
        return NULL;
    }

    start_time= RTMP_GetTime();
    //表示可以开始推流了
    readyPushing = 1;
    packets.setWork(1);
    RTMPPacket *packet = 0;
    while (readyPushing) {
       packets.get(packet);
       if(!readyPushing) {
           break;
       }
       if(!packet) {
           continue;
       }

       //设置流类型为视频流
       packet->m_nInfoField2 = rtmp->m_stream_id;
       RTMP_SendPacket(rtmp,packet,1);
       releasePackets(packet);
    }

    readyPushing = 0;
    isStart = 0;

    packets.clear();
    packets.setWork(0);
    if(rtmp) {
        RTMP_Close(rtmp);
        RTMP_Free(rtmp);
    }

    delete(url);
    return 0;
}

void releasePackets(RTMPPacket *pPacket) {
    if(pPacket) {
        RTMPPacket_Free(pPacket);
        delete pPacket;
        pPacket = 0;
    }

}


extern "C" JNIEXPORT jstring JNICALL
Java_com_example_zhangzd_zzdpush_MainActivity_stringFromJNI(JNIEnv *env, jobject instance) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}




//初始化videoChannel
extern "C" JNIEXPORT void JNICALL
Java_com_example_zhangzd_zzdpush_LivePusher_native_1init(JNIEnv *env, jobject instance) {
    videoChannel = new VideoChannel;
    videoChannel->setCallback(videoCallback);
}


extern "C" JNIEXPORT void JNICALL
Java_com_example_zhangzd_zzdpush_LivePusher_native_1setVideoEncInfo(JNIEnv *env, jobject instance,
                                                                    jint width, jint height,
                                                                    jint fps, jint bitrate) {
    if(!videoChannel) {
        return;
    }
    videoChannel->setVideoEncInfo(width,height,fps,bitrate);

}


extern "C" JNIEXPORT void JNICALL
Java_com_example_zhangzd_zzdpush_LivePusher_native_1start(JNIEnv *env, jobject instance,jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);

    //判断如果已经开启直播，则直接
    if(isStart)  {
        return;
    }
    isStart = 1; //设置标记位为开启直播

    //开线程，从队列中取出数据，通过rtmpdump发送数据


   //将path保存，防止线程开启后path被销毁
    char *url = new char[strlen(path) + 1];  //加一个结束标识位\0
    strcpy(url,path);

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