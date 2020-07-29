//
// Created by haisheng on 2020/5/11.
//

#include "HsCalljava.h"

HsCalljava::HsCalljava(_JavaVM *javaVM, JNIEnv *env, jobject obj) {
    this->javaVm = javaVM;
    this->jniEnv = env;
    this->jobj = env->NewGlobalRef(obj);

    jclass jclz = env->GetObjectClass(obj);
    if (!jclz) {
        if (LOG_DEBUG) {
            LOGE("get jclass wrong");
        }
        return;
    }
    jmid_prepare = env->GetMethodID(jclz, "onCallPrepare", "()V");
    jmid_load = env->GetMethodID(jclz, "onCallLoading", "(Z)V");
    jmid_timeinfo = env->GetMethodID(jclz, "onCallTimeInfo", "(II)V");
    jmid_error = env->GetMethodID(jclz, "onCallError", "(ILjava/lang/String;)V");

    jmid_complete = env->GetMethodID(jclz, "onCallComplete", "()V");

    jmid_renderYUV = env->GetMethodID(jclz, "onCallRenderYUV", "(II[B[B[B)V");
}

HsCalljava::~HsCalljava() {

}

void HsCalljava::onCallPrepare(int thread_type) {
    if (!jmid_prepare) {
        if (LOG_DEBUG) {
            LOGE("jmid_prepare is null");
        }
        return;
    }
    if (thread_type == MAIN_THREAD) {
        if (LOG_DEBUG) {
            LOGD("onCallPrepare MAIN_THREAD");
        }
        this->jniEnv->CallVoidMethod(this->jobj, this->jmid_prepare);
    } else {
        if (LOG_DEBUG) {
            LOGD("onCallPrepare CHILD_THREAD");
        }
        JNIEnv *jniEnv2;
        if (this->javaVm->AttachCurrentThread(&jniEnv2, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv worng");
            }
            return;
        }
        jniEnv2->CallVoidMethod(this->jobj, this->jmid_prepare);
        this->javaVm->DetachCurrentThread();
    }
}

void HsCalljava::onCallLoading(bool load, int thread_type) {
    if (!jmid_load) {
        if (LOG_DEBUG) {
            LOGE("jmid_load is null");
        }
        return;
    }
    if (thread_type == MAIN_THREAD) {
        if (LOG_DEBUG) {
            // LOGD("onCallPrepare MAIN_THREAD");
        }
        this->jniEnv->CallVoidMethod(this->jobj, this->jmid_load, load);
    } else {
        if (LOG_DEBUG) {
            //  LOGD("onCallPrepare CHILD_THREAD");
        }
        JNIEnv *jniEnv2;
        if (this->javaVm->AttachCurrentThread(&jniEnv2, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv worng");
            }
            return;
        }
        jniEnv2->CallVoidMethod(this->jobj, this->jmid_load, load);
        this->javaVm->DetachCurrentThread();
    }
}

void HsCalljava::onCallTimeInfo(int current, int total, int thread_type) {
    if (!jmid_timeinfo) {
        if (LOG_DEBUG) {
            LOGE("jmid_timeinfo is null");
        }
        return;
    }
    if (thread_type == MAIN_THREAD) {
        if (LOG_DEBUG) {
            // LOGD("onCallPrepare MAIN_THREAD");
        }
        this->jniEnv->CallVoidMethod(this->jobj, this->jmid_timeinfo, current, total);
    } else {
        if (LOG_DEBUG) {
            //  LOGD("onCallPrepare CHILD_THREAD");
        }
        JNIEnv *jniEnv2;
        if (this->javaVm->AttachCurrentThread(&jniEnv2, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv worng");
            }
            return;
        }
        jniEnv2->CallVoidMethod(this->jobj, this->jmid_timeinfo, current, total);
        this->javaVm->DetachCurrentThread();
    }
}

void HsCalljava::onCallError(int code, char *msg, int thread_type) {
    if (!jmid_error) {
        if (LOG_DEBUG) {
            LOGE("jmid_error is null");
        }
        return;
    }
    if (thread_type == MAIN_THREAD) {
        if (LOG_DEBUG) {
            // LOGD("onCallPrepare MAIN_THREAD");
        }
        jstring jmsg = jniEnv->NewStringUTF(msg);
        this->jniEnv->CallVoidMethod(this->jobj, this->jmid_error, code, jmsg);
        this->jniEnv->DeleteLocalRef(jmsg);
    } else {
        if (LOG_DEBUG) {
            //  LOGD("onCallPrepare CHILD_THREAD");
        }
        JNIEnv *jniEnv2;
        if (this->javaVm->AttachCurrentThread(&jniEnv2, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv worng");
            }
            return;
        }
        jstring jmsg = jniEnv2->NewStringUTF(msg);
        jniEnv2->CallVoidMethod(this->jobj, this->jmid_error, code, jmsg);
        jniEnv2->DeleteLocalRef(jmsg);
        this->javaVm->DetachCurrentThread();
    }
}

void HsCalljava::onCallComplete(int thread_type) {
    if (!jmid_complete) {
        if (LOG_DEBUG) {
            LOGE("jmid_complete is null");
        }
        return;
    }
    if (thread_type == MAIN_THREAD) {
        if (LOG_DEBUG) {
            LOGD("onCallComplete MAIN_THREAD");
        }
        this->jniEnv->CallVoidMethod(this->jobj, this->jmid_prepare);
    } else {
        if (LOG_DEBUG) {
            LOGD("onCallComplete CHILD_THREAD");
        }
        JNIEnv *jniEnv2;
        if (this->javaVm->AttachCurrentThread(&jniEnv2, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv worng");
            }
            return;
        }
        jniEnv2->CallVoidMethod(this->jobj, this->jmid_complete);
        this->javaVm->DetachCurrentThread();
    }
}

void HsCalljava::onCallRenderYUV(int width, int height, uint8_t *fy, uint8_t *fu, uint8_t *fv,
                                 int thread_type) {
    if (!jmid_renderYUV) {
        if (LOG_DEBUG) {
            LOGE("jmid_renderYUV is null");
        }
        return;
    }

    if (thread_type == MAIN_THREAD){

        jbyteArray y = jniEnv->NewByteArray(width*height);
        jniEnv->SetByteArrayRegion(y, 0,width*height, reinterpret_cast<const jbyte *>(fy));

        jbyteArray u = jniEnv->NewByteArray(width*height/4);
        jniEnv->SetByteArrayRegion(u, 0,width*height/4, reinterpret_cast<const jbyte *>(fu));

        jbyteArray v = jniEnv->NewByteArray(width*height/4);
        jniEnv->SetByteArrayRegion(v, 0,width*height/4, reinterpret_cast<const jbyte *>(fv));

        jniEnv->CallVoidMethod(jobj,jmid_renderYUV,width,height,y,u,v);

        jniEnv->DeleteLocalRef(y);
        jniEnv->DeleteLocalRef(u);
        jniEnv->DeleteLocalRef(v);


    }else{
        JNIEnv *jniEnv2;
        if (this->javaVm->AttachCurrentThread(&jniEnv2, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("get child thread jnienv worng");
            }
            return;
        }

        jbyteArray y = jniEnv2->NewByteArray(width*height);
        jniEnv2->SetByteArrayRegion(y, 0,width*height, reinterpret_cast<const jbyte *>(fy));

        jbyteArray u = jniEnv2->NewByteArray(width*height/4);
        jniEnv2->SetByteArrayRegion(u, 0,width*height/4, reinterpret_cast<const jbyte *>(fu));

        jbyteArray v = jniEnv2->NewByteArray(width*height/4);
        jniEnv2->SetByteArrayRegion(v, 0,width*height/4, reinterpret_cast<const jbyte *>(fv));

        jniEnv2->CallVoidMethod(jobj,jmid_renderYUV,width,height,y,u,v);

        jniEnv2->DeleteLocalRef(y);
        jniEnv2->DeleteLocalRef(u);
        jniEnv2->DeleteLocalRef(v);

        this->javaVm->DetachCurrentThread();
    }


}