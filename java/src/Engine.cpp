//
// Created by kier on 2020/7/3.
//

#include <iostream>
#include "com_seetatech_aip_Engine.h"
#include "common.h"

#include "seeta_aip_engine.h"

#include "native.h"

JNIEXPORT void JNICALL Java_com_seetatech_aip_Engine_construct
        (JNIEnv *env, jobject self, jstring libname) {
    jclass self_class = env->GetObjectClass(self);
    jfieldID self_field_handle = env->GetFieldID(self_class, "handle", "J");

    auto native_libname = jni_convert_string(env, libname);
    try {
        auto native_engine = new NativeEngine(env, native_libname);
        auto case4engine = new_defer(std::default_delete<NativeEngine>(), native_engine);

        auto native_handle = reinterpret_cast<jlong>(native_engine);
        env->SetLongField(self, self_field_handle, native_handle);
        if (env->ExceptionCheck()) return;

        case4engine.release();
    } catch (const JNIExceptionCheck &) {
        return;
    } catch (const std::exception &e) {
        jni_throw(env, e.what());
    }
}

JNIEXPORT void JNICALL Java_com_seetatech_aip_Engine_destruct
        (JNIEnv *env, jobject self) {
    jclass self_class = env->GetObjectClass(self);
    jfieldID self_field_handle = env->GetFieldID(self_class, "handle", "J");

    auto native_handle = env->GetLongField(self, self_field_handle);
    auto native_engine = reinterpret_cast<NativeEngine *>(native_handle);
    if (!native_engine) return;

    delete native_engine;
    env->SetLongField(self, self_field_handle, 0);
}

JNIEXPORT jobject JNICALL Java_com_seetatech_aip_Engine_load
        (JNIEnv *env, jobject self) {
    jclass self_class = env->GetObjectClass(self);
    jfieldID self_field_handle = env->GetFieldID(self_class, "handle", "J");

    auto native_handle = env->GetLongField(self, self_field_handle);
    auto native_engine = reinterpret_cast<NativeEngine *>(native_handle);

    auto jni_package = native_engine->aip();
    return jni_package;
}


