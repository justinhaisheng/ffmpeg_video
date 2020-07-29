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
        LOGE("子线程解码一个video  avframe成功");

        if (avFrame->format == AV_PIX_FMT_YUV420P){
            LOGE("当前视频是yuv420p");
            video->calljava->onCallRenderYUV(video->avCodecContext->width,
                                             video->avCodecContext->height,
                                             avFrame->data[0],
                                             avFrame->data[1],
                                             avFrame->data[2],
                                             CHILD_THREAD);
        } else{
            LOGE("当前视频不是yuv420p,需要转换");

            //init ctx
            SwsContext *sws_ctx = sws_getContext(video->avCodecContext->width,
                                                 video->avCodecContext->height,
                                                 video->avCodecContext->pix_fmt,
                                                 video->avCodecContext->width,
                                                 video->avCodecContext->height,
                                                 AV_PIX_FMT_YUV420P,
                                                 SWS_BICUBIC,NULL,NULL,NULL);

            if (!sws_ctx){
                continue;
            }

            AVFrame *pFrameYUV420P = av_frame_alloc();
            //AV_PIX_FMT_YUV420P 所占用的buffer size
            int num = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,
                                               video->avCodecContext->width,
                                               video->avCodecContext->height,
                                               1);
            //申请内存
            uint8_t* buffer = static_cast<uint8_t *>(av_malloc(num * sizeof(uint8_t)));
            //填充
            av_image_fill_arrays(pFrameYUV420P->data,
                                 pFrameYUV420P->linesize,
                                 buffer,AV_PIX_FMT_YUV420P,
                                 video->avCodecContext->width,
                                 video->avCodecContext->height,
                                 1);
            //转换
            sws_scale(sws_ctx,avFrame->data,
                    avFrame->linesize,
                    0,
                    video->avCodecContext->height,
                    pFrameYUV420P->data,
                    pFrameYUV420P->linesize);

            //回调到应用层进行渲染
            video->calljava->onCallRenderYUV(video->avCodecContext->width,
                                             video->avCodecContext->height,
                                             pFrameYUV420P->data[0],
                                             pFrameYUV420P->data[1],
                                             pFrameYUV420P->data[2],
                                             CHILD_THREAD);


            av_frame_free(&pFrameYUV420P);
            av_free(pFrameYUV420P);
            pFrameYUV420P = NULL;

            av_free(buffer);
            sws_freeContext(sws_ctx);
        }


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