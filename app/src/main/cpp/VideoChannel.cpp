//
// Created by zhangzd on 2019-06-19.
//

#include "VideoChannel.h"
#include "include/x264.h"

void VideoChannel::setVideoEncInfo(int width, int height, int fps, int bitRate) {

    mWidth = width;
    mHeight = height;
    mfps = fps;
    mBitRate = bitRate;
    mYSize = width * height;
    mUvSize = mYSize / 4;

    //设置编码器信息
    x264_param_t * param;
    x264_param_default_preset(param,x264_preset_names[0],x264_tune_names[7]);
    //编码复杂度
    param->i_level_idc = 32;
    //编码格式
    param->i_csp = X264_CSP_I420;
    //设置宽高
    param->i_height = height;
    param->i_width = width;
    //设置无b帧，为实现首开
    param->i_bframe = 0;

    //set rate control params
    //设置速率控制参数

    //参数i_rc_method表示码率控制，CQP(恒定质量)，CRF(恒定码率)，ABR(平均码率)
    param->rc.i_rc_method = X264_RC_ABR;
    //码率(比特率,单位Kbps)  bitrate (bps)
    param->rc.i_bitrate = bitRate / 1000;
    //瞬时最大码率   网速   1M    10M
    param->rc.i_vbv_max_bitrate = static_cast<int>(bitRate / 1000 * 1.2);
    //设置了i_vbv_max_bitrate必须设置此参数，码率控制区大小,单位kbps
    param->rc.i_vbv_buffer_size = bitRate /1000;

    param->i_fps_num = fps;
    param->i_fps_den = 1;
    param->i_timebase_den = param->i_fps_num;
    param->i_timebase_num = param->i_fps_den;

    //1 ：时间基和时间戳用于码率控制  0 ：仅帧率用于码率控制
    param->b_vfr_input = 0;
    //关键帧间隔 2s 一个关键帧
    param->i_keyint_max = fps  * 2;

    // 是否复制sps和pps放在每个关键帧的前面 该参数设置是让每个关键帧(I帧)都附带sps/pps。
    param->b_repeat_headers = 1;
    //多线程
    param->i_threads = 1;

    //添加配置文件限定
    x264_param_apply_profile(param,"baseline");
    //打开编码器
    videoCodec = x264_encoder_open(param);

//    pic_in = new x264_picture_t;  此处需不需要new

    x264_picture_alloc(pic_in,X264_CSP_I420,width,height);
}
