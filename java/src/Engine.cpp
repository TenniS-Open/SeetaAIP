//
// Created by kier on 2020/7/3.
//

#include "../include/seeta_aip_Engine.h"

#include "common.h"
#include "seeta_aip_engine.h"
#include "native.h"

JNIEXPORT void JNICALL Java_seeta_aip_Engine_construct
        (JNIEnv *env, jobject self, jstring libname) {
    jclass self_class = env->GetObjectClass(self);
    jfieldID self_field_handle = env->GetFieldID(self_class, "handle", "J");
    jfieldID self_field_aip = env->GetFieldID(self_class, "aip", "L" JNI_PACKAGE "Package;");

    auto native_libname = jni_convert_string(env, libname);
    try {
        auto native_engine = new NativeEngine(native_libname);
        auto case4engine = new_defer(std::default_delete<NativeEngine>(), native_engine);

        auto aip = native_engine->aip();
        auto java_cdata = env->NewByteArray(sizeof(aip));
        defer(&JNIEnv::DeleteLocalRef, env, java_cdata);
        env->SetByteArrayRegion(java_cdata, 0, sizeof(aip), reinterpret_cast<jbyte*>(&aip));

        auto Package_class = jni_find_class(env, "Package");
        auto Package_init = env->GetMethodID(Package_class, "<init>", "([B)V");
        auto jni_package = env->NewObject(Package_class, Package_init, java_cdata);
        if (env->ExceptionCheck()) throw JNIExceptionCheck();
        defer(&JNIEnv::DeleteLocalRef, env, jni_package);

        env->SetObjectField(self, self_field_aip, jni_package);
        if (env->ExceptionCheck()) return;

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

JNIEXPORT void JNICALL Java_seeta_aip_Engine_destruct
        (JNIEnv *env, jobject self) {
    jclass self_class = env->GetObjectClass(self);
    jfieldID self_field_handle = env->GetFieldID(self_class, "handle", "J");

    auto native_handle = env->GetLongField(self, self_field_handle);
    auto native_engine = reinterpret_cast<NativeEngine *>(native_handle);
    if (!native_engine) return;

    delete native_engine;
    env->SetLongField(self, self_field_handle, 0);
}


