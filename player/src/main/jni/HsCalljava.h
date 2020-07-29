//
// Created by haisheng on 2020/5/11.
//

#ifndef MUSIC_HSCALLJAVA_H
#define MUSIC_HSCALLJAVA_H

#include "jni.h"
#include <linux/stddef.h>
#include "AndroidLog.h"

#define MAIN_THREAD 0
#define CHILD_THREAD 1

class HsCalljava {
public:
    _JavaVM* javaVm = NULL;
    JNIEnv *jniEnv = NULL;
    jobject jobj = NULL;

    jmethodID  jmid_prepare;
    jmethodID  jmid_load;
    jmethodID  jmid_timeinfo;
    jmethodID  jmid_error;
    jmethodID  jmid_complete;
    jmethodID  jmid_renderYUV;
public:
    HsCalljava(_JavaVM *javaVM, JNIEnv *env, jobject obj);
    ~HsCalljava();

    void onCallPrepare(int thread_type);
    void onCallLoading(bool load,int thread_type);
    void onCallTimeInfo(int current,int total,int thread_type);
    void onCallError(int code,char* msg,int thread_type);
    void onCallComplete(int thread_type);
    void onCallRenderYUV(int width,int height,uint8_t* fy,uint8_t* fu,uint8_t* fv,int thread_type);
};


#endif //MUSIC_HSCALLJAVA_H
