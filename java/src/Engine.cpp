//
// Created by kier on 2020/7/3.
//

#include "com_seetatech_aip_Engine.h"
#include "common.h"

#include "seeta_aip_engine.h"
#include "jvmti.h"

using NativeEngine = seeta::aip::Engine;

JNIEXPORT void JNICALL Java_com_seetatech_aip_Engine_construct
        (JNIEnv *env, jobject self, jstring libname) {
    jclass self_class = env->GetObjectClass(self);
    jfieldID self_field_handle = env->GetFieldID(self_class, "handle", "J");

    auto native_libname = jni_convert_string(env, libname);
    try {
        auto native_engine = new NativeEngine(native_libname);
        auto case4engine = new_defer(std::default_delete<NativeEngine>(), native_engine);

        auto native_handle = reinterpret_cast<jlong>(native_engine);
        env->SetLongField(self, self_field_handle, native_handle);
        if (env->ExceptionCheck()) return;

        case4engine.release();
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

    delete native_engine;
}


