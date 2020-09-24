//
// Created by kier on 2020/9/24.
//

#ifndef SEETA_AIP_JARRAY_H
#define SEETA_AIP_JARRAY_H

#include "JavaHelper.h"

template<>
class JConverter<jint> {
public:
    JNIEnv *env;

    explicit JConverter(JNIEnv *env)
        : env(env) {}

    using NativeObject = jint;

    AutoJObject convert_array(const std::vector<NativeObject> &array) const {
        auto N = jsize(array.size());
        AutoJObject result(env, env->NewIntArray(N));
        env->SetIntArrayRegion(result.get<jintArray>(), 0, N, array.data());
        return result;
    }

    AutoJObject convert_array(const NativeObject *data, jsize N) const {
        AutoJObject result(env, env->NewIntArray(N));
        env->SetIntArrayRegion(result.get<jintArray>(), 0, N, data);
        return result;
    }

    template <typename T, typename=typename std::enable_if<
            !std::is_same<T, NativeObject>::value &&
            std::is_integral<T>::value &&
            sizeof(T) == 4>::type>
    AutoJObject convert_array(const std::vector<T> &array) const {
        return convert_array(
                reinterpret_cast<const NativeObject *>(array.data()),
                jsize(array.size()));
    }

    template <typename T, typename=typename std::enable_if<
            !std::is_same<T, NativeObject>::value &&
            std::is_integral<T>::value &&
            sizeof(T) == 4>::type>
    AutoJObject convert_array(const T *data, jsize N) const {
        return convert_array(
                reinterpret_cast<const NativeObject *>(data),
                N);
    }

    std::vector<NativeObject> convert_array(jintArray array) const {
        if (!array) return {};
        auto N = env->GetArrayLength(array);
        std::vector<NativeObject> result(N);
        env->GetIntArrayRegion(array, 0, N, result.data());
        return result;
    }

    template <typename T, typename=typename std::enable_if<
            !std::is_same<T, NativeObject>::value &&
            std::is_integral<T>::value &&
            sizeof(T) == 4>::type>
    std::vector<T> convert_array(jintArray array) const {
        if (!array) return {};
        auto N = env->GetArrayLength(array);
        std::vector<T> result(N);
        env->GetIntArrayRegion(array, 0, N, reinterpret_cast<NativeObject*>(result.data()));
        return result;
    }
};

template<>
class JConverter<jbyte> {
public:
    JNIEnv *env;

    explicit JConverter(JNIEnv *env)
            : env(env) {}

    using NativeObject = jbyte;

    AutoJObject convert_array(const std::vector<NativeObject> &array) const {
        auto N = jsize(array.size());
        AutoJObject result(env, env->NewByteArray(N));
        env->SetByteArrayRegion(result.get<jbyteArray>(), 0, N, array.data());
        return result;
    }

    AutoJObject convert_array(const NativeObject *data, jsize N) const {
        AutoJObject result(env, env->NewByteArray(N));
        env->SetByteArrayRegion(result.get<jbyteArray>(), 0, N, data);
        return result;
    }

    template <typename T, typename=typename std::enable_if<
            !std::is_same<T, NativeObject>::value &&
            std::is_integral<T>::value &&
            sizeof(T) == 1>::type>
    AutoJObject convert_array(const std::vector<T> &array) const {
        return convert_array(
                reinterpret_cast<const NativeObject *>(array.data()),
                jsize(array.size()));
    }

    template <typename T, typename=typename std::enable_if<
            !std::is_same<T, NativeObject>::value &&
            std::is_integral<T>::value &&
            sizeof(T) == 1>::type>
    AutoJObject convert_array(const T *data, jsize N) const {
        return convert_array(
                reinterpret_cast<const NativeObject *>(data),
                N);
    }

    std::vector<NativeObject> convert_array(jbyteArray array) const {
        if (!array) return {};
        auto N = env->GetArrayLength(array);
        std::vector<NativeObject> result(N);
        env->GetByteArrayRegion(array, 0, N, result.data());
        return result;
    }

    template <typename T, typename=typename std::enable_if<
            !std::is_same<T, NativeObject>::value &&
            std::is_integral<T>::value &&
            sizeof(T) == 1>::type>
    std::vector<T> convert_array(jbyteArray array) const {
        if (!array) return {};
        auto N = env->GetArrayLength(array);
        std::vector<T> result(N);
        env->GetByteArrayRegion(array, 0, N, reinterpret_cast<NativeObject*>(result.data()));
        return result;
    }
};

template<>
class JConverter<jfloat> {
public:
    JNIEnv *env;

    explicit JConverter(JNIEnv *env)
            : env(env) {}

    using NativeObject = jfloat;

    AutoJObject convert_array(const std::vector<NativeObject> &array) const {
        auto N = jsize(array.size());
        AutoJObject result(env, env->NewFloatArray(N));
        env->SetFloatArrayRegion(result.get<jfloatArray>(), 0, N, array.data());
        return result;
    }

    AutoJObject convert_array(const NativeObject *data, jsize N) const {
        AutoJObject result(env, env->NewFloatArray(N));
        env->SetFloatArrayRegion(result.get<jfloatArray>(), 0, N, data);
        return result;
    }

    std::vector<NativeObject> convert_array(jfloatArray array) const {
        if (!array) return {};
        auto N = env->GetArrayLength(array);
        std::vector<NativeObject> result(N);
        env->GetFloatArrayRegion(array, 0, N, result.data());
        return result;
    }
};

template<>
class JConverter<jdouble> {
public:
    JNIEnv *env;

    explicit JConverter(JNIEnv *env)
            : env(env) {}

    using NativeObject = jdouble;

    AutoJObject convert_array(const std::vector<NativeObject> &array) const {
        auto N = jsize(array.size());
        AutoJObject result(env, env->NewDoubleArray(N));
        env->SetDoubleArrayRegion(result.get<jdoubleArray>(), 0, N, array.data());
        return result;
    }

    AutoJObject convert_array(const NativeObject *data, jsize N) const {
        AutoJObject result(env, env->NewDoubleArray(N));
        env->SetDoubleArrayRegion(result.get<jdoubleArray>(), 0, N, data);
        return result;
    }

    std::vector<NativeObject> convert_array(jdoubleArray array) const {
        if (!array) return {};
        auto N = env->GetArrayLength(array);
        std::vector<NativeObject> result(N);
        env->GetDoubleArrayRegion(array, 0, N, result.data());
        return result;
    }
};

using JInt = JConverter<jint>;
using JByte = JConverter<jbyte>;
using JFloat = JConverter<jfloat>;
using JDouble = JConverter<jdouble>;

#endif //SEETA_AIP_JARRAY_H
