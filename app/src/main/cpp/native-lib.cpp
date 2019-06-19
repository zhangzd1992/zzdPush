#include <jni.h>
#include <string>
#include <pthread.h>
#include "x264.h"
#include "librtmp/rtmp.h"
#include "VideoChannel.h"
#include "macro.h"


VideoChannel *videoChannel;

int isStart = 0;
//线程索引
pthread_t* pid;


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



}


extern "C" JNIEXPORT jstring JNICALL
Java_com_example_zhangzd_zzdpush_MainActivity_stringFromJNI(JNIEnv *env, jobject instance) {

    x264_picture_t* x264_picture = new x264_picture_t;
    RTMP_Alloc();

    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}




//初始化videoChannel
extern "C" JNIEXPORT void JNICALL
Java_com_example_zhangzd_zzdpush_LivePusher_native_1init(JNIEnv *env, jobject instance) {

    videoChannel = new VideoChannel;



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

    pthread_create(pid, 0, threadStart, reinterpret_cast<void *>(url));



    env->ReleaseStringUTFChars(path_, path);
}