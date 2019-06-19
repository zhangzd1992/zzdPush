//
// Created by zhangzd on 2019-06-19.
//

#include "include/x264.h"

#ifndef ZZDPUSH_VIDEOCHANNEL_H


class VideoChannel {

public:
    void setVideoEncInfo(int width, int height, int fps, int bitRate);
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

};
#endif //ZZDPUSH_VIDEOCHANNEL_H
