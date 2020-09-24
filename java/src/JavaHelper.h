#pragma once

#include <jni.h>
#include <algorithm>
#include <string>
#include <vector>

#include "common.h"

#define JNI_PACKAGE "seeta/aip/"

using JNIEnvPtr = JNIEnv *;

class AutoJObject
{
public:
    AutoJObject() {}
    AutoJObject(JNIEnvPtr env, jobject jobj) : env(env), jobj(jobj) {}
	~AutoJObject() { if (env && jobj) env->DeleteLocalRef(jobj); }

    AutoJObject(const AutoJObject &other) = delete;
    const AutoJObject &operator=(const AutoJObject &other) = delete;

	AutoJObject(AutoJObject &&other) { std::swap(this->jobj, other.jobj); }
	const AutoJObject &operator=(AutoJObject &&other) { std::swap(this->jobj, other.jobj); return *this; }

	operator jobject() const { return jobj; }

	jobject get() { return jobj; }
    const jobject get() const { return jobj; }

    template <typename JT>
    JT get() { return static_cast<JT>(this->get()); }

    template <typename JT>
    const JT get() const { return static_cast<JT>(this->get()); }

	jobject release()
	{
		jobject jobj = this->jobj;
		this->jobj = nullptr;
		return jobj;
	}

    template <typename JT>
    JT release() { return static_cast<JT>(this->release()); }

	operator bool() const { return jobj != nullptr; }

private:
	JNIEnvPtr env = nullptr;
	jobject jobj = nullptr;
};

class JClass
{
public:
	jclass clazz = nullptr;
	JNIEnv *env;
    std::string clazz_name;
	explicit JClass(JNIEnv *env, const char *name)
	{
		this->env = env;
		this->clazz = env->FindClass(name);
        this->clazz_name = name;
	}

    explicit JClass(JNIEnv *env, jclass clazz)
    {
        this->env = env;
        this->clazz = clazz;
        this->clazz_name = "self";
    }

    explicit JClass(JNIEnv *env, jobject obj)
    {
        this->env = env;
        this->clazz = env->GetObjectClass(obj);
        this->clazz_name = "self";
    }

	jfieldID GetFieldID(const char *name, const char *sig) const
	{
		return env->GetFieldID(clazz, name, sig);
	}

    jobject construct() const {
        auto ctor = env->GetMethodID(clazz, "<init>", "()V");
        return env->NewObject(clazz, ctor);
    }

    template <typename... Args>
    jobject construct(const std::string &sig, Args &&... args) const {
	    auto ctor = env->GetMethodID(clazz, "<init>", sig.c_str());
	    return env->NewObject(clazz, ctor, std::forward<Args>(args)...);
	}
};

template <typename T>
class JConverter : public JClass {
public:
    using NativeObject = T;

    using JClass::JClass;

    virtual AutoJObject convert(const NativeObject &object) const = 0;

    virtual NativeObject convert(jobject object) const = 0;

    AutoJObject convert_array(const std::vector<NativeObject> &array) {
        auto N = jsize(array.size());
        AutoJObject result(env, env->NewObjectArray(N, clazz, nullptr));
        for (jsize i = 0; i < N; ++i) {
            env->SetObjectArrayElement(result.get<jobjectArray>(), i, convert(array[i]));
        }
        return result;
    }

    std::vector<NativeObject> convert_array(jobjectArray array) {
        std::vector<NativeObject> native_array;
        auto N = env->GetArrayLength(array);
        native_array.reserve(N);
        for (jsize i = 0; i < N; ++i) {
            auto obj = env->GetObjectArrayElement(array, i);
            defer_delete_local_ref(env, obj);
            native_array.emplace_back(convert(obj));
        }
        return native_array;
    }
};

