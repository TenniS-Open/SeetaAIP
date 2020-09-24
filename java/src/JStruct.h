#pragma once

#include "JavaHelper.h"
#include "JEnum.h"
#include "seeta_aip_struct.h"
#include "common.h"
#include "JArray.h"

class JPoint : public JConverter<seeta::aip::Point> {
public:
    jfieldID x;
    jfieldID y;

    explicit JPoint(JNIEnv *env)
            : JConverter(env, JNI_PACKAGE "Point") {
        x = GetFieldID("x", "F");
        y = GetFieldID("y", "F");
    }

    AutoJObject convert(const NativeObject &object) const final {
        return AutoJObject(env, construct("(FF)V", jfloat(object.x), jfloat(object.y)));
    }

    NativeObject convert(jobject object) const final {
        return {env->GetFloatField(object, x),
                env->GetFloatField(object, y)};
    }
};

class JShapeType : public JEnum {
public:
    explicit JShapeType(JNIEnv *env)
    : JEnum(env, JNI_PACKAGE "ShapeType")
    {
        this->bind("Unknown", SEETA_AIP_UNKNOWN_SHAPE);
        this->bind("Points", SEETA_AIP_POINTS);
        this->bind("Lines", SEETA_AIP_LINES);
        this->bind("Rectangle", SEETA_AIP_RECTANGLE);
        this->bind("Parallelogram", SEETA_AIP_PARALLELOGRAM);
        this->bind("Polygon", SEETA_AIP_POLYGON);
        this->bind("Circle", SEETA_AIP_CIRCLE);
        this->bind("Cube", SEETA_AIP_CUBE);
        this->bind("NoShape", SEETA_AIP_NO_SHAPE);
    }
};

class JShape : public JConverter<seeta::aip::Shape> {
public:
    jfieldID type;
    jfieldID landmarks;
    jfieldID rotate;
    jfieldID scale;
    explicit JShape(JNIEnv *env)
            : JConverter(env, JNI_PACKAGE "Shape") {
        type = env->GetFieldID(clazz, "type", "L" JNI_PACKAGE "ShapeType;");
        landmarks = env->GetFieldID(clazz, "landmarks", "[L" JNI_PACKAGE "Point;");
        rotate = env->GetFieldID(clazz, "rotate", "F");
        scale = env->GetFieldID(clazz, "scale", "F");
    }

    AutoJObject convert(const NativeObject &object) const final {
        JShapeType clazz_shape_type(env);
        JPoint clazz_point(env);

        auto java_type = clazz_shape_type.convert(int(object.type()));
        auto java_landmarks = clazz_point.convert_array(object.landmarks());
        auto java_rotate = jfloat(object.rotate());
        auto java_scale = jfloat(object.scale());

        auto java_shape = AutoJObject(env, construct());
        env->SetObjectField(java_shape, type, java_type);
        env->SetObjectField(java_shape, landmarks, java_landmarks);
        env->SetFloatField(java_shape, rotate, java_rotate);
        env->SetFloatField(java_shape, scale, java_scale);

        return java_shape;
    }

    NativeObject convert(jobject object) const final {
        JShapeType clazz_shape_type(env);
        JPoint clazz_point(env);
        
        auto java_type = env->GetObjectField(object, type);
        defer(&JNIEnv::DeleteLocalRef, env, java_type);
        auto java_landmarks = env->GetObjectField(object, landmarks);
        defer(&JNIEnv::DeleteLocalRef, env, java_landmarks);
        auto java_rotate = env->GetFloatField(object, rotate);
        auto java_scale = env->GetFloatField(object, scale);
        
        auto native_type = clazz_shape_type.convert(java_type);
        auto native_landmarks = clazz_point.convert_array(reinterpret_cast<jobjectArray>(java_landmarks));
        auto native_rotate = float(java_rotate);
        auto native_scale = float(java_scale);

        seeta::aip::Shape result;
        result.type(SEETA_AIP_SHAPE_TYPE(native_type));
        result.landmarks(native_landmarks);
        result.rotate(native_rotate);
        result.scale(native_scale);

        return result;
    }
};

class JValueType : public JEnum {
public:
    explicit JValueType(JNIEnv *env)
            : JEnum(env, JNI_PACKAGE "ValueType")
    {
        this->bind("Void", SEETA_AIP_VALUE_VOID);
        this->bind("Byte", SEETA_AIP_VALUE_BYTE);
        this->bind("Int", SEETA_AIP_VALUE_INT32);
        this->bind("Float", SEETA_AIP_VALUE_FLOAT32);
        this->bind("Double", SEETA_AIP_VALUE_FLOAT64);
    }
};

class JTensor : public JConverter<seeta::aip::Tensor> {
public:
    jfieldID type;
    jfieldID data_float;
    jfieldID data_byte;
    jfieldID data_int;
    jfieldID data_double;
    jfieldID dims;

    explicit JTensor(JNIEnv *env)
            : JConverter(env, JNI_PACKAGE "Tensor") {
        type = env->GetFieldID(clazz, "type", "L" JNI_PACKAGE "ValueType;");
        data_float = env->GetFieldID(clazz, "data_float", "[F");
        data_byte = env->GetFieldID(clazz, "data_byte", "[B");
        data_int = env->GetFieldID(clazz, "data_int", "[I");
        data_double = env->GetFieldID(clazz, "data_double", "[D");
        dims = env->GetFieldID(clazz, "dims", "[I");
    }

    AutoJObject convert(const NativeObject &object) const final {
        JValueType clazz_value_type(env);

        auto java_type = clazz_value_type.convert(int(object.type()));
        auto java_dims = JInt(env).convert_array(object.dims());

        auto java_tensor = AutoJObject(env, construct());
        auto &native_dims = object.dims();
        auto N = jsize(std::accumulate(
                native_dims.begin(), native_dims.end(),
                1, std::multiplies<uint32_t>()));

        env->SetObjectField(java_tensor, type, java_type);
        env->SetObjectField(java_tensor, dims, java_dims);

        switch (object.type()) {
            default:
                break;
            case SEETA_AIP_VALUE_BYTE:
                env->SetObjectField(
                        java_tensor, data_byte,
                        JByte(env).convert_array(object.data<jbyte>(), N));
                break;
            case SEETA_AIP_VALUE_INT32:
                env->SetObjectField(
                        java_tensor, data_int,
                        JInt(env).convert_array(object.data<jint>(), N));
                break;
            case SEETA_AIP_VALUE_FLOAT32:
                env->SetObjectField(
                        java_tensor, data_float,
                        JFloat(env).convert_array(object.data<jfloat>(), N));
                break;
            case SEETA_AIP_VALUE_FLOAT64:
                env->SetObjectField(
                        java_tensor, data_double,
                        JDouble(env).convert_array(object.data<jdouble>(), N));
                break;
        }

        return java_tensor;
    }

    NativeObject convert(jobject object) const final {
        if (!object) return NativeObject();

        JValueType clazz_value_type(env);

        auto java_type = env->GetObjectField(object, type);
        defer(&JNIEnv::DeleteLocalRef, env, java_type);
        auto java_dims = env->GetObjectField(object, dims);
        defer(&JNIEnv::DeleteLocalRef, env, java_dims);

        auto native_type = clazz_value_type.convert(java_type);
        auto native_dims = JInt(env).convert_array<uint32_t>(reinterpret_cast<jintArray>(java_dims));
        auto N = jsize(std::accumulate(
                native_dims.begin(), native_dims.end(),
                1, std::multiplies<uint32_t>()));

        NativeObject result(SEETA_AIP_VALUE_TYPE(native_type), native_dims);

        switch (native_type) {
            default:
                break;
            case SEETA_AIP_VALUE_BYTE:
            {
                auto java_data = reinterpret_cast<jbyteArray>(env->GetObjectField(object, data_byte));
                defer_delete_local_ref(env, java_data);
                env->GetByteArrayRegion(java_data, 0, N, result.data<jbyte>());
                break;
            }
            case SEETA_AIP_VALUE_INT32:
            {
                auto java_data = reinterpret_cast<jintArray>(env->GetObjectField(object, data_int));
                defer_delete_local_ref(env, java_data);
                env->GetIntArrayRegion(java_data, 0, N, result.data<jint>());
                break;
            }
            case SEETA_AIP_VALUE_FLOAT32:
            {
                auto java_data = reinterpret_cast<jfloatArray>(env->GetObjectField(object, data_float));
                defer_delete_local_ref(env, java_data);
                env->GetFloatArrayRegion(java_data, 0, N, result.data<jfloat>());
                break;
            }
            case SEETA_AIP_VALUE_FLOAT64:
            {
                auto java_data = reinterpret_cast<jdoubleArray>(env->GetObjectField(object, data_double));
                defer_delete_local_ref(env, java_data);
                env->GetDoubleArrayRegion(java_data, 0, N, result.data<jdouble>());
                break;
            }
        }

        return result;
    }
};

class JTag : public JConverter<seeta::aip::Object::Tag> {
public:
    jfieldID label;
    jfieldID score;

    explicit JTag(JNIEnv *env)
            : JConverter(env, JNI_PACKAGE "Object$Tag") {
        label = GetFieldID("label", "I");
        score = GetFieldID("score", "F");
    }

    AutoJObject convert(const NativeObject &object) const final {
        return AutoJObject(env, construct("(IF)V", jint(object.label), jfloat(object.score)));
    }

    NativeObject convert(jobject object) const final {
        return {env->GetIntField(object, label),
                env->GetFloatField(object, score)};
    }
};


class JAIPObject : public JConverter<seeta::aip::Object> {
public:
    jfieldID shape;
    jfieldID tags;
    jfieldID extra;
    explicit JAIPObject(JNIEnv *env)
            : JConverter(env, JNI_PACKAGE "Object") {
        shape = env->GetFieldID(clazz, "shape", "L" JNI_PACKAGE "Shape;");
        tags = env->GetFieldID(clazz, "tags", "[L" JNI_PACKAGE "Object$Tag;");
        extra = env->GetFieldID(clazz, "extra", "L" JNI_PACKAGE "Tensor;");
    }

    AutoJObject convert(const NativeObject &object) const final {
        JShape clazz_shape(env);
        JTag clazz_tag(env);
        JTensor clazz_tensor(env);

        auto java_shape = clazz_shape.convert(object.shape());
        auto java_tags = clazz_tag.convert_array(object.tags());
        auto java_extra = clazz_tensor.convert(object.extra());

        auto java_object = AutoJObject(env, construct());
        env->SetObjectField(java_object, shape, java_shape);
        env->SetObjectField(java_object, tags, java_tags);
        env->SetObjectField(java_object, extra, java_extra);

        return java_object;
    }

    NativeObject convert(jobject object) const final {
        JShape clazz_shape(env);
        JTag clazz_tag(env);
        JTensor clazz_tensor(env);

        auto java_shape = env->GetObjectField(object, shape);
        defer(&JNIEnv::DeleteLocalRef, env, java_shape);
        auto java_tags = env->GetObjectField(object, tags);
        defer(&JNIEnv::DeleteLocalRef, env, java_tags);
        auto java_extra = env->GetObjectField(object, extra);
        defer(&JNIEnv::DeleteLocalRef, env, java_extra);


        auto native_shape = clazz_shape.convert(java_shape);
        auto native_tags = clazz_tag.convert_array(reinterpret_cast<jobjectArray>(java_tags));
        auto native_extra = clazz_tensor.convert(java_extra);

        seeta::aip::Object result(native_shape, native_tags, native_extra);

        return result;
    }
};

class JDevice : public JConverter<seeta::aip::Device> {
public:
    jfieldID device;
    jfieldID id;

    explicit JDevice(JNIEnv *env)
            : JConverter(env, JNI_PACKAGE "Device") {
        device = GetFieldID("device", "Ljava/lang/String;");
        id = GetFieldID("id", "I");
    }

    AutoJObject convert(const NativeObject &object) const final {
        return AutoJObject(
                env, construct("(Ljava/lang/String;I)V",
                               jobject(JString(env).convert(object.device())),
                               jint(object.id())));
    }

    NativeObject convert(jobject object) const final {
        auto java_device = env->GetObjectField(object, device);
        defer_delete_local_ref(env, java_device);
        return {JString(env).convert(java_device),
                env->GetIntField(object, id)};
    }
};
