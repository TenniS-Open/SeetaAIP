//
// Created by kier on 2020/7/3.
//

#include "../include/seeta_aip_Package.h"

#include "native.h"
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
 * Class:     seeta_aip_Package
 * Method:    construct
 * Signature: ([B)V
 */
JNIEXPORT void JNICALL Java_seeta_aip_Package_construct
        (JNIEnv *env, jobject self, jbyteArray cdata) {
    jclass self_class = env->GetObjectClass(self);
    jfieldID self_field_cdata = env->GetFieldID(self_class, "cdata", "[B");
    jfieldID self_field_aip_version = env->GetFieldID(self_class, "aip_version", "I");
    jfieldID self_field_module = env->GetFieldID(self_class, "module", "Ljava/lang/String;");
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

    env->SetIntField(self, self_field_aip_version, aip.aip_version);
    env->SetObjectField(self, self_field_module, jni_convert_string(env, aip.module));
    env->SetObjectField(self, self_field_description, jni_convert_string(env, aip.description));
    env->SetObjectField(self, self_field_mID, jni_convert_string(env, aip.mID));
    env->SetObjectField(self, self_field_sID, jni_convert_string(env, aip.sID));
    env->SetObjectField(self, self_field_version, jni_convert_string(env, aip.version));

    jclass JString_class = env->FindClass("java/lang/String");
    auto support_size = array_size(aip.support, nullptr);
    auto jni_support = env->NewObjectArray(support_size, JString_class, nullptr);
    defer(&JNIEnv::DeleteLocalRef, env, jni_support);
    for (int i = 0; i < support_size; ++i) {
        env->SetObjectArrayElement(jni_support, i, jni_convert_string(env, aip.support[i]));
    }
    env->SetObjectField(self, self_field_support, jni_support);
}

/*
 * Class:     seeta_aip_Package
 * Method:    error
 * Signature: (JI)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_seeta_aip_Package_error
        (JNIEnv *env, jobject self, jlong handle, jint errorCode) {
    auto instance = reinterpret_cast<NativeInstance*>(handle);
    try {
        return jni_convert_string(env, instance->error(errorCode)).release<jstring>();
    } catch (const JNIExceptionCheck &) {
    }  catch (const seeta::aip::Exception &e) {
        jni_throw_aip_exception(env, e.errcode(), e.what());
    } catch (const std::exception &e) {
        jni_throw(env, e.what());
    }
    return nullptr;
}

/*
 * Class:     seeta_aip_Package
 * Method:    create
 * Signature: ([Ljava/lang/String;[Lseeta/aip/Object;)J
 */
JNIEXPORT jlong JNICALL Java_seeta_aip_Package_create
        (JNIEnv *env, jobject self, jobjectArray models, jobjectArray objects) {
//    try {
//        return jni_convert_string(env, instance->error(errorCode)).release<jstring>();
//    } catch (const JNIExceptionCheck &) {
//    }  catch (const seeta::aip::Exception &e) {
//        jni_throw_aip_exception(env, e.errcode(), e.what());
//    } catch (const std::exception &e) {
//        jni_throw(env, e.what());
//    }
    return 0;
}
