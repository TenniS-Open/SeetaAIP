#include "JEnum.h"
#include "JString.h"
#include "common.h"

#include "seeta_aip_struct.h"

#include <sstream>

namespace {
    template <typename T>
    inline void SCat(std::ostream &out, T &&t) {
        out << std::forward<T>(t);
    }
    template <typename T, typename... Args>
    inline void SCat(std::ostream &out, T &&t, Args &&...args) {
        out << std::forward<T>(t);
        SCat(out, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline std::string Cat(Args &&...args) {
        std::ostringstream oss;
        SCat(oss, std::forward<Args>(args)...);
        return oss.str();
    }
}

JEnum::JEnum(JNIEnv* env, const char* name)
    : JClass(env, name)
{
}

JEnum::~JEnum()
{
}

void JEnum::bind(const std::string& jvalue, int value)
{
    java2local[jvalue] = value;
    local2java[value] = jvalue;
}

AutoJObject JEnum::convert(int value) const
{
    auto it = local2java.find(value);
    if (it == local2java.end())
    {
        throw seeta::aip::Exception(Cat("Got an local ", value, " for ", value));
    }
    std::string value_string = it->second;

    jfieldID field_value = env->GetStaticFieldID(clazz, value_string.c_str(),
                                                 Cat("L", clazz_name, ";").c_str());

    jobject jvalue = env->GetStaticObjectField(clazz, field_value);

    return AutoJObject(env, jvalue);
}

int JEnum::convert(jobject jvalue) const
{
    JString clazz_string(env);
    jmethodID getVal = env->GetMethodID(clazz, "name", "()Ljava/lang/String;");
    auto value_string = static_cast<jstring>(env->CallObjectMethod(jvalue, getVal));
    defer(&JNIEnv::DeleteLocalRef, env, value_string);
    std::string local_value_string = clazz_string.convert(value_string);

    auto it = java2local.find(local_value_string);
    if (it == java2local.end())
    {
        throw seeta::aip::Exception(Cat("Got an java ", local_value_string, " for "));
    }

    return it->second;
}
