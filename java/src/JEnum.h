#pragma once

#include "JavaHelper.h"
#include <map>

class JEnum : public JClass
{
public:
    JEnum(JNIEnv *env, const char *name);
    ~JEnum();

    void bind(const std::string &jvalue, int value);

    AutoJObject convert(int value) const;
    int convert(jobject jvalue) const;

private:
    std::map<int, std::string> local2java;
    std::map<std::string, int> java2local;
};

