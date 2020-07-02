//
// Created by haisheng on 2020/7/1.
//


#include "HsVideo.h"

HsVideo::HsVideo(HsPlaystatus* playstatus,HsCalljava* calljava) {
    this->playstatus = playstatus;
    this->calljava = calljava;
    queue = new HsQueue(playstatus);
}

HsVideo::~HsVideo() {

}


void* playVideo(void* data){
    HsVideo* video = static_cast<HsVideo *>(data);
    while (video->playstatus&&!video->playstatus->exit){
        if (video->queue->getQueueSize() == 0){//没有数据
            if (!video->playstatus->load){
                video->playstatus->load = true;
                video->calljava->onCallLoading(video->playstatus->load,CHILD_THREAD);
            }
            av_usleep(1000*100);
            continue;
        }
        AVPacket* avPacket = av_packet_alloc();
        if (video->queue->getPacket(avPacket) == 0){
            //解码渲染
            LOGE("video线程中获取视频AVpacket");
        }
        av_packet_free(&avPacket);
        av_free(avPacket);
        avPacket = NULL;
    }
    pthread_exit(&video->thread_play);
}

void HsVideo::play() {
    pthread_create(&thread_play,NULL,playVideo,this);
}