//
// Created by kier on 2020/8/5.
//

#ifndef SEETA_AIP_JAVA_COMMON_H
#define SEETA_AIP_JAVA_COMMON_H

#include <jni.h>
#include <string>
#include <functional>

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

std::string jni_convert_string(JNIEnv *env, jstring jni_string);

inline void jni_throw(JNIEnv* env, const std::string &msg)
{
    jclass Exception_class = env->FindClass("java/lang/Exception");
    env->ThrowNew(Exception_class, msg.c_str());
}




#endif //SEETA_AIP_JAVA_COMMON_H
