//
// Created by kier on 2020/8/5.
//

#ifndef SEETA_AIP_JAVA_NATIVE_H
#define SEETA_AIP_JAVA_NATIVE_H

#include "seeta_aip_engine.h"

class NativePackage {
public:
    using self = NativePackage;

    explicit NativePackage(std::shared_ptr<seeta::aip::Engine> engine)
        : m_engine(std::move(engine)) {}

    explicit NativePackage(const std::string &libname)
        : m_engine(std::make_shared<seeta::aip::Engine>(libname)) {}

    const SeetaAIP &aip() const { return m_engine->aip(); }

private:
    std::shared_ptr<seeta::aip::Engine> m_engine;
};

class NativeEngine {
public:
    explicit NativeEngine(JNIEnv *env, const std::string &libname) {
        m_engine = std::make_shared<seeta::aip::Engine>(libname);
        auto native_package = new NativePackage(m_engine);
        auto case4package = new_defer(std::default_delete<NativePackage>(), native_package);

        auto jni_package_handle = reinterpret_cast<jlong>(native_package);

        auto Package_class = jni_find_class(env, "Package");
        auto Package_init = env->GetMethodID(Package_class, "<init>", "(J)V");
        auto jni_package = env->NewObject(Package_class, Package_init, jni_package_handle);
        if (env->ExceptionCheck()) throw JNIExceptionCheck();
        defer(&JNIEnv::DeleteLocalRef, env, jni_package);
        case4package.release();

        m_package = env->NewGlobalRef(jni_package);
        if (env->ExceptionCheck()) throw JNIExceptionCheck();
        m_env = env;
    }

    ~NativeEngine() {
        m_env->DeleteGlobalRef(m_package);
    }

    jobject aip() const {
        return m_package;
    }

private:
    std::shared_ptr<seeta::aip::Engine> m_engine;
    jobject m_package;
    JNIEnv *m_env;
};

#endif //SEETA_AIP_JAVA_NATIVE_H
