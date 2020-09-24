#pragma once

#include <jni.h>
#include <string>
#include <memory>
#include <cstring>

#include "JavaHelper.h"

void jni_throw(JNIEnv* env, const std::string& msg);

void jni_throw(JNIEnv* env, const std::string &clazz, const std::string& msg);

void jni_throw_aip_exception(JNIEnv *env, int errcode, const std::string &msg);