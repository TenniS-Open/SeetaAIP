//
// Created by kier on 2020/8/5.
//

#ifndef SEETA_AIP_JAVA_NATIVE_H
#define SEETA_AIP_JAVA_NATIVE_H

#include "seeta_aip_engine.h"

using NativeEngine = seeta::aip::Engine;
using NativeInstance = seeta::aip::Instance;

//
//class NativeEngine {
//public:
//    explicit NativeEngine(JNIEnv *env, const std::string &libname) {
//        m_engine = std::make_shared<seeta::aip::Engine>(libname);
//
//        auto Package_class = jni_find_class(env, "Package");
//        auto Package_init = env->GetMethodID(Package_class, "<init>", "([B)V");
//
//        auto aip = m_engine->aip();
//        auto java_cdata = env->NewByteArray(sizeof(aip));
//        defer(&JNIEnv::DeleteLocalRef, env, java_cdata);
//
//        env->SetByteArrayRegion(java_cdata, 0, sizeof(aip), reinterpret_cast<jbyte*>(&aip));
//
//        auto jni_package = env->NewObject(Package_class, Package_init, java_cdata);
//        if (env->ExceptionCheck()) throw JNIExceptionCheck();
//        defer(&JNIEnv::DeleteLocalRef, env, jni_package);
//
//        m_package = env->NewGlobalRef(jni_package);
//        if (env->ExceptionCheck()) throw JNIExceptionCheck();
//        m_env = env;
//    }
//
//    ~NativeEngine() {
//        m_env->DeleteGlobalRef(m_package);
//    }
//
//    jobject aip() const {
//        return m_package;
//    }
//
//private:
//    std::shared_ptr<seeta::aip::Engine> m_engine;
//    jobject m_package;
//    JNIEnv *m_env;
//};

#endif //SEETA_AIP_JAVA_NATIVE_H
