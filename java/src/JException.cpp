//
// Created by kier on 2020/9/24.
//

#include "JException.h"
#include "JString.h"
#include "common.h"

void jni_throw(JNIEnv* env, const std::string& msg)
{
    jclass Exception_class = env->FindClass("java/lang/RuntimeException");
    env->ThrowNew(Exception_class, msg.c_str());
}

void jni_throw(JNIEnv* env, const std::string& clazz, const std::string& msg)
{
    jclass Exception_class = env->FindClass(clazz.c_str());
    env->ThrowNew(Exception_class, msg.c_str());
}

void jni_throw_aip_exception(JNIEnv *env, int errcode, const std::string &msg) {
    auto exception_class = env->FindClass(JNI_PACKAGE "Exception");
    auto exception_init = env->GetMethodID(exception_class, "<init>", "(ILjava/lang/String;)V");
    auto exception = env->NewObject(exception_class, exception_init,
                                    errcode, jni_convert_string(env, msg).get());
    defer(&JNIEnv::DeleteLocalRef, env, exception);
    env->Throw(reinterpret_cast<jthrowable>(exception));
}
