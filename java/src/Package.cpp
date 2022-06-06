//
// Created by kier on 2020/7/3.
//

#include "../include/seeta_aip_Package.h"

#include "seeta_aip.h"

#include "native.h"
#include "JString.h"
#include "JStruct.h"
#include "JException.h"

#include <iostream>

template <typename T, typename K,
        typename=typename std::enable_if<std::is_constructible<T, K>::value>::type>
static inline int array_size(T *array, K env_val) {
    int size = 0;
    const T end(env_val);
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
    env->SetObjectField(self, self_field_cdata, cdata);
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
    auto instance = (NativeInstance*)(handle);
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
 * Signature: (Lseeta/aip/Device;[Ljava/lang/String;[Lseeta/aip/Object;)J
 */
JNIEXPORT jlong JNICALL Java_seeta_aip_Package_create
        (JNIEnv *env, jobject self, jobject device, jobjectArray models, jobjectArray objects) {
    jclass self_class = env->GetObjectClass(self);
    jfieldID self_field_cdata = env->GetFieldID(self_class, "cdata", "[B");
    auto jni_cdata = reinterpret_cast<jbyteArray>(env->GetObjectField(self, self_field_cdata));
    jbyte* native_cdata = env->GetByteArrayElements(jni_cdata, JNI_FALSE);
    defer(&JNIEnv::ReleaseByteArrayElements, env, jni_cdata, native_cdata, 0);
    auto &aip = *reinterpret_cast<SeetaAIP*>(native_cdata);
    try {
        auto native_device = JDevice(env).convert(device);
        auto native_models = JString(env).convert_array(models);
        auto native_objects = JAIPObject(env).convert_array(objects);
        std::unique_ptr<NativeInstance> native_instance(
                new NativeInstance(aip, native_device, native_models, native_objects));
        return (jlong)(native_instance.release());
    } catch (const JNIExceptionCheck &) {
    }  catch (const seeta::aip::Exception &e) {
        jni_throw_aip_exception(env, e.errcode(), e.what());
    } catch (const std::exception &e) {
        jni_throw(env, e.what());
    }
    return 0;
}
/*
 * Class:     seeta_aip_Package
 * Method:    free
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_seeta_aip_Package_free
        (JNIEnv *env, jobject self, jlong handle) {
    auto instance = (NativeInstance*)(handle);
    if (!instance) return;
    delete instance;
}

/*
 * Class:     seeta_aip_Package
 * Method:    property
 * Signature: (J)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_seeta_aip_Package_property
        (JNIEnv *env, jobject self, jlong handle) {
    auto instance = (NativeInstance*)(handle);
    try {
        JString clazz_string(env);
        return clazz_string.convert_array(instance->property()).release<jobjectArray>();
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
 * Method:    setd
 * Signature: (JLjava/lang/String;D)V
 */
JNIEXPORT void JNICALL Java_seeta_aip_Package_setd
        (JNIEnv *env, jobject self, jlong handle, jstring name, jdouble value) {
    auto instance = (NativeInstance*)(handle);
    try {
        JString clazz_string(env);
        instance->setd(clazz_string.convert(name), double(value));
    } catch (const JNIExceptionCheck &) {
    }  catch (const seeta::aip::Exception &e) {
        jni_throw_aip_exception(env, e.errcode(), e.what());
    } catch (const std::exception &e) {
        jni_throw(env, e.what());
    }
}

/*
 * Class:     seeta_aip_Package
 * Method:    getd
 * Signature: (JLjava/lang/String;)D
 */
JNIEXPORT jdouble JNICALL Java_seeta_aip_Package_getd
        (JNIEnv *env, jobject self, jlong handle, jstring name) {
    auto instance = (NativeInstance*)(handle);
    try {
        JString clazz_string(env);
        return jdouble(instance->getd(clazz_string.convert(name)));
    } catch (const JNIExceptionCheck &) {
    }  catch (const seeta::aip::Exception &e) {
        jni_throw_aip_exception(env, e.errcode(), e.what());
    } catch (const std::exception &e) {
        jni_throw(env, e.what());
    }
    return 0;
}

/*
 * Class:     seeta_aip_Package
 * Method:    reset
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_seeta_aip_Package_reset
        (JNIEnv *env, jobject self, jlong handle) {
    auto instance = (NativeInstance*)(handle);
    try {
        instance->reset();
    } catch (const JNIExceptionCheck &) {
    }  catch (const seeta::aip::Exception &e) {
        jni_throw_aip_exception(env, e.errcode(), e.what());
    } catch (const std::exception &e) {
        jni_throw(env, e.what());
    }
}

/*
 * Class:     seeta_aip_Package
 * Method:    forward
 * Signature: (JI[Lseeta/aip/ImageData;[Lseeta/aip/Object;)Lseeta/aip/Result;
 */
JNIEXPORT jobject JNICALL Java_seeta_aip_Package_forward
        (JNIEnv *env, jobject self, jlong handle, jint methodID, jobjectArray images, jobjectArray objects) {
    auto instance = (NativeInstance*)(handle);
    try {
        auto native_method_id = uint32_t(methodID);
        auto native_images = JImageData(env).convert_array(images);
        auto native_objects = JAIPObject(env).convert_array(objects);
        auto result = instance->forward(native_method_id,
                                        native_images,
                                        native_objects);
        return JResult(env).convert(result).release();
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
 * Method:    tag
 * Signature: (JIII)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_seeta_aip_Package_tag
        (JNIEnv *env, jobject self, jlong handle, jint methodID, jint labelIndex, jint labelValue) {
    auto instance = (NativeInstance*)(handle);
    try {
        auto native_method_id = uint32_t(methodID);
        auto native_index = uint32_t(labelIndex);
        auto native_value = int32_t(labelValue);
        return JString(env).convert(instance->tag(
                native_method_id, native_index, native_value
                )).release<jstring>();
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
 * Method:    set
 * Signature: (JLjava/lang/String;Lseeta/aip/Object;)V
 */
JNIEXPORT void JNICALL Java_seeta_aip_Package_set
        (JNIEnv *env, jobject self, jlong handle, jstring name, jobject value) {
    auto instance = (NativeInstance*)(handle);
    try {
        auto native_name = JString(env).convert(name);
        auto native_value = JAIPObject(env).convert(value);
        instance->set(native_name, native_value);
    } catch (const JNIExceptionCheck &) {
    }  catch (const seeta::aip::Exception &e) {
        jni_throw_aip_exception(env, e.errcode(), e.what());
    } catch (const std::exception &e) {
        jni_throw(env, e.what());
    }
}

/*
 * Class:     seeta_aip_Package
 * Method:    get
 * Signature: (JLjava/lang/String;)Lseeta/aip/Object;
 */
JNIEXPORT jobject JNICALL Java_seeta_aip_Package_get
        (JNIEnv *env, jobject self, jlong handle, jstring name) {
    auto instance = (NativeInstance*)(handle);
    try {
        auto native_name = JString(env).convert(name);
        auto native_value = instance->get(native_name);
        return JAIPObject(env).convert(native_value).release();
    } catch (const JNIExceptionCheck &) {
    }  catch (const seeta::aip::Exception &e) {
        jni_throw_aip_exception(env, e.errcode(), e.what());
    } catch (const std::exception &e) {
        jni_throw(env, e.what());
    }
    return nullptr;
}