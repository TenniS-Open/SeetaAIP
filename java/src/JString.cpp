//
// Created by kier on 2020/9/24.
//

#include "JString.h"
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

std::vector<std::string> jni_convert_string_array(JNIEnv *env, jobjectArray jni_array) {
    std::vector<std::string> native_array;
    auto N = env->GetArrayLength(jni_array);
    native_array.reserve(N);
    for (jsize i = 0; i < N; ++i) {
        auto obj = env->GetObjectArrayElement(jni_array, i);
        native_array.emplace_back(jni_convert_string(env, reinterpret_cast<jstring>(obj)));
    }
    return native_array;
}

AutoJObject jni_convert_string_array(JNIEnv *env, const std::vector<std::string> &array) {
    jclass JString_class = env->FindClass("java/lang/String");
    auto N = jsize(array.size());
    AutoJObject result(env, env->NewObjectArray(N, JString_class, nullptr));
    for (jsize i = 0; i < N; ++i) {
        env->SetObjectArrayElement(result.get<jobjectArray>(), i, jni_convert_string(env, array[i]));
    }
    return result;
}
