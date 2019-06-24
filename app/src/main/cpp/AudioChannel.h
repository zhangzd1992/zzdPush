//
// Created by zhangzd on 2019-06-24.
//

#ifndef ZZDPUSH_AUDIOCHANNEL_H
#define ZZDPUSH_AUDIOCHANNEL_H


#include <faac.h>
#include <jni.h>
#include <pty.h>
#include "librtmp/rtmp.h"
#include "librtmp/bytes.h"

class AudioChannel {
    typedef void (*AudioCallback)(RTMPPacket *packet);
private:
    unsigned long inputSamples;
    unsigned long maxOutputBytes;
    faacEncHandle  audioCodec;
    u_char *buffer;
    int mChannels;
    AudioCallback callback;
    ~AudioChannel();


public:
    /**
     * 设置编码信息
     * @param sampleRateInHz
     * @param channels
     */
    void setAudioInfo(int sampleRateInHz, int channels);
    long getInputSamples();

    void encodeData(int8_t *data);
    void setAudioCallback(AudioCallback audioCallback);
    RTMPPacket * getAudioTag();
};


#endif //ZZDPUSH_AUDIOCHANNEL_H
