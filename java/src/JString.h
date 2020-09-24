//
// Created by kier on 2020/9/24.
//

#ifndef SEETA_AIP_JSTRING_H
#define SEETA_AIP_JSTRING_H

#include "JavaHelper.h"

#include <vector>

std::string jni_convert_string(JNIEnv *env, jstring jni_string);

AutoJObject jni_convert_string(JNIEnv *env, const std::string& str);

std::vector<std::string> jni_convert_string_array(JNIEnv *env, jobjectArray jni_array);

AutoJObject jni_convert_string_array(JNIEnv *env, const std::vector<std::string> &array);

class JString : public JConverter<std::string> {
public:
    explicit JString(JNIEnv *env)
            : JConverter(env, "java/lang/String") {
    }

    using NativeObject = std::string;

    AutoJObject convert(const NativeObject &object) const final {
        return jni_convert_string(env, object);
    }

    NativeObject convert(jobject object) const final {
        return jni_convert_string(env, reinterpret_cast<jstring>(object));
    }
};



#endif //SEETA_AIP_JSTRING_H
