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

        if(video->playstatus->seek){
            av_usleep(1000*100);
            continue;
        }

        if (video->queue->getQueueSize() == 0){//没有数据
            if (!video->playstatus->load){
                video->playstatus->load = true;
                video->calljava->onCallLoading(video->playstatus->load,CHILD_THREAD);
            }
            av_usleep(1000*100);
            continue;
        }

        if(video->playstatus->load)
        {
            video->playstatus->load = false;
            video->calljava->onCallLoading(video->playstatus->load, CHILD_THREAD);
        }

        AVPacket* avPacket = av_packet_alloc();
        if (video->queue->getPacket(avPacket) != 0){
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            av_usleep(1000*100);
            continue;
        }

        //解码
        if(avcodec_send_packet(video->avCodecContext,avPacket)!=0){
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            av_usleep(1000*100);
            continue;
        }
        AVFrame* avFrame = av_frame_alloc();
        if (avcodec_receive_frame(video->avCodecContext,avFrame)!=0){
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;


            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;

            av_usleep(1000*100);
            continue;
        }
        LOGE("子线程解码一个video  AVframe成功");



        av_packet_free(&avPacket);
        av_free(avPacket);
        avPacket = NULL;


        av_frame_free(&avFrame);
        av_free(avFrame);
        avFrame = NULL;
    }
    pthread_exit(&video->thread_play);
}

void HsVideo::play() {
    pthread_create(&thread_play,NULL,playVideo,this);
}

void HsVideo::release() {
    if (queue){
        delete queue;
        queue =NULL;
    }
    if (playstatus){
        playstatus = NULL;
    }
    if (calljava){
        calljava = NULL;
    }
    if(avCodecContext){
        avcodec_close(avCodecContext);
        avcodec_free_context(&avCodecContext);
        avCodecContext = NULL;
    }
}