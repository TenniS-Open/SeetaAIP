//
// Created by kier on 2020/8/5.
//

#ifndef SEETA_AIP_JAVA_COMMON_H
#define SEETA_AIP_JAVA_COMMON_H

#include <jni.h>
#include <string>
#include <functional>
#include <vector>

class JNIExceptionCheck : public std::exception {};

template <typename T>
class Defer {
public:
    using Actor = std::function<T()>;

    template <typename FUNC, typename... Args>
    explicit Defer(FUNC func, Args &&...args)
        : m_actor(std::bind(func, std::forward<Args>(args)...)) {}

    ~Defer() {
        if (m_actor) m_actor();
    }

    void release() {
        m_actor = nullptr;
    }

    void emit() {
        if (m_actor) m_actor();
        m_actor = nullptr;
    }

private:
    Actor m_actor;
};

template <typename FUNC, typename... Args>
inline auto new_defer(FUNC func, Args &&...args) -> Defer<decltype(std::bind(func, std::forward<Args>(args)...)())> {
    using T = decltype(std::bind(func, std::forward<Args>(args)...)());
    return Defer<T>(func, std::forward<Args>(args)...);
}

#define __javac_concat__(x, y) x##y

#define __javac_concat(x, y) __javac_concat__(x, y)

#define __javac_defer_name __javac_concat(__javac_defer_, __LINE__)

#define defer(func, ...) auto __javac_defer_name = new_defer(func, ## __VA_ARGS__)

#define JNI_PACKAGE "seeta/aip/"

using JNIEnvPtr = JNIEnv *;

class AutoJObject
{
public:
    AutoJObject() = default;
    AutoJObject(JNIEnvPtr env, jobject jobj) : env(env), jobj(jobj) {}
    ~AutoJObject() { if (env && jobj) env->DeleteLocalRef(jobj); }

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

    AutoJObject(const AutoJObject &other) = delete;
    const AutoJObject &operator=(const AutoJObject &other) = delete;

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
    jfieldID GetFieldID(const char *name, const char *sig) const
    {
        return env->GetFieldID(clazz, name, sig);
    }
};


inline std::string jni_package() {
    return JNI_PACKAGE;
}

inline jclass jni_find_class(JNIEnv *env, const std::string &classname) {
    auto fullname = jni_package() + classname;
    return env->FindClass(fullname.c_str());
}

inline void jni_throw(JNIEnv* env, const std::string &msg)
{
    jclass Exception_class = env->FindClass("java/lang/RuntimeException");
    env->ThrowNew(Exception_class, msg.c_str());
}

void jni_throw_aip_exception(JNIEnv* env, int errcode, const std::string &msg);

std::string jni_convert_string(JNIEnv *env, jstring jni_string);
AutoJObject jni_convert_string(JNIEnv *env, const std::string& str);

std::vector<std::string> jni_convert_array_string(JNIEnv *env, jobjectArray jni_array);





#endif //SEETA_AIP_JAVA_COMMON_H
