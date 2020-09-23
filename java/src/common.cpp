//
// Created by kier on 2020/8/5.
//

#include "common.h"

#include <memory>

std::string jni_convert_string(JNIEnv *env, jstring jni_string) {
    std::unique_ptr<char[]> buffer;
    jclass JString_class = env->FindClass("java/lang/String");

    jstring jni_utf8 = env->NewStringUTF("utf-8");
    defer(&JNIEnv::DeleteLocalRef, env, jni_utf8);

    jmethodID JString_method_getBytes = env->GetMethodID(JString_class, "getBytes", "(Ljava/lang/String;)[B");

    auto jni_byte_array = reinterpret_cast<jbyteArray>(env->CallObjectMethod(jni_string, JString_method_getBytes, jni_utf8));
    defer(&JNIEnv::DeleteLocalRef, env, jni_byte_array);

    jsize native_array_length = env->GetArrayLength(jni_byte_array);
    jbyte* native_array = env->GetByteArrayElements(jni_byte_array, JNI_FALSE);
    defer(&JNIEnv::ReleaseByteArrayElements, env, jni_byte_array, native_array, 0);

    if (native_array_length > 0)
    {
        buffer.reset(new char[native_array_length + 1]);
        memcpy(buffer.get(), native_array, native_array_length);
        buffer[native_array_length] = '\0';
    }

    return buffer.get();
}

AutoJObject jni_convert_string(JNIEnv *env, const std::string &str) {
    return AutoJObject(env, env->NewStringUTF(str.c_str()));
}

void jni_throw_aip_exception(JNIEnv *env, int errcode, const std::string &msg) {
    auto exception_class = jni_find_class(env, "Exception");
    auto exception_init = env->GetMethodID(exception_class, "<init>", "(ILjava/lang/String;)V");
    auto exception = env->NewObject(exception_class, exception_init,
                                    errcode, jni_convert_string(env, msg).get());
    defer(&JNIEnv::DeleteLocalRef, env, exception);
    env->Throw(reinterpret_cast<jthrowable>(exception));
}
