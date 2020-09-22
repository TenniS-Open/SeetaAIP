//
// Created by kier on 2020/7/3.
//

#include "com_seetatech_aip_Package.h"

#include "common.h"
#include "seeta_aip.h"
#include <iostream>

template <typename T, typename K,
        typename=typename std::enable_if<std::is_constructible<T, K>::value>::type>
static inline int array_size(T *array, K env_val) {
    int size = 0;
    constexpr T end(env_val);
    while (array[size] != end) {
        ++size;
    }
    return size;
}


/*
 * Class:     com_seetatech_aip_Package
 * Method:    construct
 * Signature: ([B)V
 */
JNIEXPORT void JNICALL Java_com_seetatech_aip_Package_construct
        (JNIEnv *env, jobject self, jbyteArray cdata) {
    jclass self_class = env->GetObjectClass(self);
    jfieldID self_field_cdata = env->GetFieldID(self_class, "cdata", "[B");
    jfieldID self_field_description = env->GetFieldID(self_class, "description", "Ljava/lang/String;");
    jfieldID self_field_mID = env->GetFieldID(self_class, "mID", "Ljava/lang/String;");
    jfieldID self_field_sID = env->GetFieldID(self_class, "sID", "Ljava/lang/String;");
    jfieldID self_field_version = env->GetFieldID(self_class, "version", "Ljava/lang/String;");
    jfieldID self_field_support = env->GetFieldID(self_class, "support", "[Ljava/lang/String;");

    // set base data
    env->SetObjectField(self_class, self_field_cdata, cdata);
    // set other member
    jbyte* native_cdata = env->GetByteArrayElements(cdata, JNI_FALSE);
    defer(&JNIEnv::ReleaseByteArrayElements, env, cdata, native_cdata, 0);
    auto &aip = *reinterpret_cast<SeetaAIP*>(native_cdata);

    env->SetObjectField(self, self_field_description, Java_convert_string(env, aip.description));
    env->SetObjectField(self, self_field_mID, Java_convert_string(env, aip.mID));
    env->SetObjectField(self, self_field_sID, Java_convert_string(env, aip.sID));
    env->SetObjectField(self, self_field_version, Java_convert_string(env, aip.version));

    jclass JString_class = env->FindClass("java/lang/String");
    auto support_size = array_size(aip.support, nullptr);
    auto jni_support = env->NewObjectArray(support_size, JString_class, nullptr);
    defer(&JNIEnv::DeleteLocalRef, env, jni_support);
    for (int i = 0; i < support_size; ++i) {
        env->SetObjectArrayElement(jni_support, i, Java_convert_string(env, aip.support[i]));
    }
    env->SetObjectField(self, self_field_support, jni_support);
}

