#include <jni.h>
#include <string>

#include "com_haisheng_player_HsPlay.h"

#include "AndroidLog.h"
#include "HsFFmpeg.h"
#include "HsCalljava.h"

extern "C"
{
#include <libavformat/avformat.h>
}



HsFFmpeg* ffmpeg = NULL;
HsCalljava* calljava = NULL;
HsPlaystatus* playstatus = NULL;
_JavaVM* javaVM = NULL;

bool stop = false;

extern "C"
JNIEXPORT void JNICALL Java_com_haisheng_player_HsPlay_n_1prepare
        (JNIEnv *env, jobject instance, jstring jsource){

    const char* source = env->GetStringUTFChars(jsource,0);
    if (!ffmpeg){
        if(!calljava){
            calljava = new HsCalljava(javaVM,env,instance);
        }
        playstatus = new HsPlaystatus();
        ffmpeg = new HsFFmpeg(calljava,playstatus,source);
    }
    stop = false;
    LOGD("prepare %d",stop);
    ffmpeg->prepare();
    env->ReleaseStringUTFChars(jsource,source);
}

extern "C"
JNIEXPORT void JNICALL Java_com_haisheng_player_HsPlay_n_1start
        (JNIEnv *, jobject){
    if (!ffmpeg){
        if (LOG_DEBUG){
            LOGE("ffmpeg wrong");
        }
        return ;
    }

    ffmpeg->start();


}

extern "C"
JNIEXPORT void JNICALL Java_com_haisheng_player_HsPlay_n_1resume
        (JNIEnv *env, jobject instance){
    if(ffmpeg){
        ffmpeg->resume();
    }
}

extern "C"
JNIEXPORT void JNICALL Java_com_haisheng_player_HsPlay_n_1pause
        (JNIEnv *env, jobject instance){
    if(ffmpeg){
        ffmpeg->pause();
    }
}

extern "C"
JNIEXPORT void JNICALL Java_com_haisheng_player_HsPlay_n_1stop
        (JNIEnv *, jobject){

    LOGE("stopping %d",stop);

    if(stop){
        if (LOG_DEBUG){
            LOGE("%s","had stopped");
        }
        return;
    }
    stop = true;
    if(ffmpeg){
        ffmpeg->release();
        delete ffmpeg;
        ffmpeg = NULL;

        if (calljava){
            delete calljava;
            calljava = NULL;
        }

        if (playstatus){
            delete playstatus;
            playstatus = NULL;
        }
    }
    stop = false;
}

extern "C"
JNIEXPORT void JNICALL Java_com_haisheng_player_HsPlay_n_1seek
        (JNIEnv *, jobject instance, jint ses){
    if(ffmpeg){
        ffmpeg->seek(ses);
    }
}

extern "C"
JNIEXPORT void JNICALL Java_com_haisheng_player_HsPlay_n_1seekVolume
        (JNIEnv *, jobject, jint volume){
    if(ffmpeg){
        ffmpeg->seekVolume(volume);
    }
}

extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved){
    jint result = -1;
    javaVM = vm;
    JNIEnv *env;
    if(vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK)
    {
        return result;
    }
    return JNI_VERSION_1_4;
}
