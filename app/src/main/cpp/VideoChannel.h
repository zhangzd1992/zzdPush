//
// Created by zhangzd on 2019-06-19.
//

#include "include/x264.h"
#include "librtmp/rtmp.h"

#ifndef ZZDPUSH_VIDEOCHANNEL_H


class VideoChannel {
    typedef void (*VideoCallback)(RTMPPacket* packet);
public:
    void setVideoEncInfo(int width, int height, int fps, int bitRate);

    void encodeData(int8_t * data);
    void setCallback(VideoCallback callback);

private:

    int mWidth;
    int mHeight;
    int mfps;
    int mBitRate;
    int mYSize;
    int mUvSize;

    x264_t * videoCodec;
    // 一帧数据
    x264_picture_t *pic_in;

    void sendSpsPps(uint8_t sps[100], uint8_t pps[100], int len, int pps_len);

    VideoCallback  videoCallback;

    void sendFrame(int type, uint8_t *p_payload, int i_payload);
};
#endif //ZZDPUSH_VIDEOCHANNEL_H
