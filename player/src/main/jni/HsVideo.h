//
// Created by haisheng on 2020/7/1.
//

#ifndef FFMPEG_VIDEO_HSVIDEO_H
#define FFMPEG_VIDEO_HSVIDEO_H

#include "HsPlaystatus.h"
#include "HsCalljava.h"
#include "HsQueue.h"

extern "C"{
#include <libswscale/swscale.h>
#include "include/libavcodec/avcodec.h"
#include <libavutil/time.h>
#include <libavutil/imgutils.h>

};


class HsVideo {
public:
    int streamIndex = -1;//流 id
    AVCodecContext* avCodecContext;//编码器上下文
    AVCodecParameters* avCodecParameters;//编码器参数
    HsPlaystatus* playstatus = NULL;
    HsCalljava* calljava = NULL;
    HsQueue* queue =NULL;
    AVRational FRAME_TIME_BASE;//frame的单位时间

    pthread_t thread_play;
public:
    HsVideo(HsPlaystatus* playstatus,HsCalljava* calljava);
    ~HsVideo();
    void play();
    void release();
};


#endif //FFMPEG_VIDEO_HSVIDEO_H
