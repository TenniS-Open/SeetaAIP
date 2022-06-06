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
            : JEnum(env, JNI_PACKAGE "ShapeType") {
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

    AutoJObject convert(const SeetaAIPShape &object) const {
        JShapeType clazz_shape_type(env);
        JPoint clazz_point(env);

        auto java_type = clazz_shape_type.convert(int(object.type));
        auto java_landmarks = clazz_point.convert_array(object.landmarks.data, jsize(object.landmarks.size));
        auto java_rotate = jfloat(object.rotate);
        auto java_scale = jfloat(object.scale);

        auto java_shape = AutoJObject(env, construct());
        env->SetObjectField(java_shape, type, java_type);
        env->SetObjectField(java_shape, landmarks, java_landmarks);
        env->SetFloatField(java_shape, rotate, java_rotate);
        env->SetFloatField(java_shape, scale, java_scale);

        return java_shape;
    }

    AutoJObject convert(const NativeObject &object) const final {
        return convert(SeetaAIPShape(object));
    }

    using JConverter::convert_array;

    AutoJObject convert_array(const SeetaAIPShape *array, jsize N) const {
        if (!array) N = 0;
        AutoJObject result(env, env->NewObjectArray(N, clazz, nullptr));
        for (jsize i = 0; i < N; ++i) {
            env->SetObjectArrayElement(result.get<jobjectArray>(), i, convert(array[i]));
        }
        return result;
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
            : JEnum(env, JNI_PACKAGE "ValueType") {
        this->bind("Void", SEETA_AIP_VALUE_VOID);
        this->bind("Byte", SEETA_AIP_VALUE_BYTE);
        this->bind("Int", SEETA_AIP_VALUE_INT32);
        this->bind("Float", SEETA_AIP_VALUE_FLOAT32);
        this->bind("Double", SEETA_AIP_VALUE_FLOAT64);
        this->bind("Char", SEETA_AIP_VALUE_CHAR);
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
    jfieldID data_string;

    explicit JTensor(JNIEnv *env)
            : JConverter(env, JNI_PACKAGE "Tensor") {
        type = env->GetFieldID(clazz, "type", "L" JNI_PACKAGE "ValueType;");
        data_float = env->GetFieldID(clazz, "data_float", "[F");
        data_byte = env->GetFieldID(clazz, "data_byte", "[B");
        data_int = env->GetFieldID(clazz, "data_int", "[I");
        data_double = env->GetFieldID(clazz, "data_double", "[D");
        dims = env->GetFieldID(clazz, "dims", "[I");
        data_string = env->GetFieldID(clazz, "data_string", "Ljava/lang/String;");
    }

    AutoJObject convert(const SeetaAIPObject::Extra &object) const {
        JValueType clazz_value_type(env);

        auto java_type = clazz_value_type.convert(int(object.type));
        auto java_dims = JInt(env).convert_array(object.dims.data, object.dims.size);

        auto java_tensor = AutoJObject(env, construct());
        auto N = jsize(std::accumulate(
                object.dims.data, object.dims.data + object.dims.size,
                1, std::multiplies<uint32_t>()));

        env->SetObjectField(java_tensor, type, java_type);
        env->SetObjectField(java_tensor, dims, java_dims);

        switch (object.type) {
            default:
                break;
            case SEETA_AIP_VALUE_BYTE:
                static_assert(sizeof(jbyte) == 1, "jbyte must be int8_t or uint8_t");
                env->SetObjectField(
                        java_tensor, data_byte,
                    JByte(env).convert_array(reinterpret_cast<const uint8_t*>(object.data), N));
                break;
            case SEETA_AIP_VALUE_INT32:
                static_assert(sizeof(jint) == 4, "jint must be int32_t or uint32_t");
                env->SetObjectField(
                        java_tensor, data_int,
                        JInt(env).convert_array(reinterpret_cast<const uint32_t*>(object.data), N));
                break;
            case SEETA_AIP_VALUE_FLOAT32:
                env->SetObjectField(
                        java_tensor, data_float,
                        JFloat(env).convert_array(reinterpret_cast<const float*>(object.data), N));
                break;
            case SEETA_AIP_VALUE_FLOAT64:
                env->SetObjectField(
                        java_tensor, data_double,
                        JDouble(env).convert_array(reinterpret_cast<const double*>(object.data), N));
                break;
            case SEETA_AIP_VALUE_CHAR:
            {
                auto str = JString(env).convert(std::string((char *)object.data, N));
                // set java's length, in case of mismatch
                auto strLen = (uint32_t)env->GetStringLength(str.get<jstring>());
                auto strDims = JInt(env).convert_array(&strLen, 1);
                env->SetObjectField(java_tensor, dims, strDims);
                env->SetObjectField(java_tensor, data_string, str);
                break;
            }
        }

        return java_tensor;
    }

    AutoJObject convert(const NativeObject &object) const final {
        return convert(SeetaAIPObject::Extra(object));
    }

    using JConverter::convert_array;

    AutoJObject convert_array(const SeetaAIPObject::Extra *array, jsize N) const {
        if (!array) N = 0;
        AutoJObject result(env, env->NewObjectArray(N, clazz, nullptr));
        for (jsize i = 0; i < N; ++i) {
            env->SetObjectArrayElement(result.get<jobjectArray>(), i, convert(array[i]));
        }
        return result;
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
            case SEETA_AIP_VALUE_BYTE: {
                static_assert(sizeof(jbyte) == 1, "jbyte must be int8_t or uint8_t");
                auto java_data = reinterpret_cast<jbyteArray>(env->GetObjectField(object, data_byte));
                defer_delete_local_ref(env, java_data);
                env->GetByteArrayRegion(java_data, 0, N, result.data<jbyte>());
                break;
            }
            case SEETA_AIP_VALUE_INT32: {
                static_assert(sizeof(jint) == 4, "jint must be int32_t or uint32_t");
                auto java_data = reinterpret_cast<jintArray>(env->GetObjectField(object, data_int));
                defer_delete_local_ref(env, java_data);
                env->GetIntArrayRegion(java_data, 0, N, result.data<jint>());
                break;
            }
            case SEETA_AIP_VALUE_FLOAT32: {
                auto java_data = reinterpret_cast<jfloatArray>(env->GetObjectField(object, data_float));
                defer_delete_local_ref(env, java_data);
                env->GetFloatArrayRegion(java_data, 0, N, result.data<jfloat>());
                break;
            }
            case SEETA_AIP_VALUE_FLOAT64: {
                auto java_data = reinterpret_cast<jdoubleArray>(env->GetObjectField(object, data_double));
                defer_delete_local_ref(env, java_data);
                env->GetDoubleArrayRegion(java_data, 0, N, result.data<jdouble>());
                break;
            }
            case SEETA_AIP_VALUE_CHAR: {
                auto java_data = reinterpret_cast<jstring>(env->GetObjectField(object, data_string));
                defer_delete_local_ref(env, java_data);
                result = seeta::aip::Tensor(JString(env).convert(java_data));
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
            : JConverter(env, JNI_PACKAGE "Tag") {
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
        tags = env->GetFieldID(clazz, "tags", "[L" JNI_PACKAGE "Tag;");
        extra = env->GetFieldID(clazz, "extra", "L" JNI_PACKAGE "Tensor;");
    }

    AutoJObject convert(const SeetaAIPObject &object) const {
        JShape clazz_shape(env);
        JTag clazz_tag(env);
        JTensor clazz_tensor(env);

        auto java_shape = clazz_shape.convert(object.shape);
        auto java_tags = clazz_tag.convert_array(object.tags.data, object.tags.size);
        auto java_extra = clazz_tensor.convert(object.extra);

        auto java_object = AutoJObject(env, construct());
        env->SetObjectField(java_object, shape, java_shape);
        env->SetObjectField(java_object, tags, java_tags);
        env->SetObjectField(java_object, extra, java_extra);

        return java_object;
    }

    AutoJObject convert(const NativeObject &object) const final {
        return convert(SeetaAIPObject(object));
    }

    using JConverter::convert_array;

    AutoJObject convert_array(const SeetaAIPObject *array, jsize N) const {
        if (!array) N = 0;
        AutoJObject result(env, env->NewObjectArray(N, clazz, nullptr));
        for (jsize i = 0; i < N; ++i) {
            env->SetObjectArrayElement(result.get<jobjectArray>(), i, convert(array[i]));
        }
        return result;
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

    AutoJObject convert(const SeetaAIPDevice &object) const {
        return AutoJObject(
                env, construct("(Ljava/lang/String;I)V",
                               jobject(JString(env).convert(object.device)),
                               jint(object.id)));
    }

    AutoJObject convert(const NativeObject &object) const final {
        return convert(SeetaAIPDevice(object));
    }

    NativeObject convert(jobject object) const final {
        auto java_device = env->GetObjectField(object, device);
        defer_delete_local_ref(env, java_device);
        return {JString(env).convert(java_device),
                env->GetIntField(object, id)};
    }
};

class JImageFormat : public JEnum {
public:
    explicit JImageFormat(JNIEnv *env)
            : JEnum(env, JNI_PACKAGE "ImageFormat") {
        this->bind("U8Raw", SEETA_AIP_FORMAT_U8RAW);
        this->bind("F32Raw", SEETA_AIP_FORMAT_F32RAW);
        this->bind("I32Raw", SEETA_AIP_FORMAT_I32RAW);
        this->bind("U8Rgb", SEETA_AIP_FORMAT_U8RGB);
        this->bind("U8Bgr", SEETA_AIP_FORMAT_U8BGR);
        this->bind("U8Rgba", SEETA_AIP_FORMAT_U8RGBA);
        this->bind("U8Bgra", SEETA_AIP_FORMAT_U8BGRA);
        this->bind("U8Y", SEETA_AIP_FORMAT_U8Y);
    }
};

class JImageData : public JConverter<seeta::aip::ImageData> {
public:
    jfieldID format;
    jfieldID type;
    jfieldID data_float;
    jfieldID data_byte;
    jfieldID data_int;
    jfieldID number;
    jfieldID height;
    jfieldID width;
    jfieldID channels;


    explicit JImageData(JNIEnv *env)
            : JConverter(env, JNI_PACKAGE "ImageData") {
        format = env->GetFieldID(clazz, "format", "L" JNI_PACKAGE "ImageFormat;");
        type = env->GetFieldID(clazz, "type", "L" JNI_PACKAGE "ValueType;");
        data_float = env->GetFieldID(clazz, "data_float", "[F");
        data_byte = env->GetFieldID(clazz, "data_byte", "[B");
        data_int = env->GetFieldID(clazz, "data_int", "[I");
        number = env->GetFieldID(clazz, "number", "I");
        height = env->GetFieldID(clazz, "height", "I");
        width = env->GetFieldID(clazz, "width", "I");
        channels = env->GetFieldID(clazz, "channels", "I");
    }

    AutoJObject convert(const SeetaAIPImageData &object) const {
        JImageFormat clazz_image_format(env);
        JValueType clazz_value_type(env);

        auto native_type = seeta::aip::ImageData::GetType(SEETA_AIP_IMAGE_FORMAT(object.format));
        auto java_format = clazz_image_format.convert(object.format);
        auto java_type = clazz_value_type.convert(native_type);
        auto java_number = jint(object.number);
        auto java_height = jint(object.height);
        auto java_width = jint(object.width);
        auto java_channels = jint(object.channels);

        auto N = jsize(java_number * java_height * java_width * java_channels);

        auto java_image = AutoJObject(env, construct());

        env->SetObjectField(java_image, format, java_format);
        env->SetObjectField(java_image, type, java_type);
        env->SetIntField(java_image, number, java_number);
        env->SetIntField(java_image, height, java_height);
        env->SetIntField(java_image, width, java_width);
        env->SetIntField(java_image, channels, java_channels);

        switch (native_type) {
            default:
                break;
            case SEETA_AIP_VALUE_BYTE:
                static_assert(sizeof(jbyte) == 1, "jbyte must be int8_t or uint8_t");
                env->SetObjectField(
                        java_image, data_byte,
                        JByte(env).convert_array(reinterpret_cast<const uint8_t*>(object.data), N));
                break;
            case SEETA_AIP_VALUE_INT32:
                static_assert(sizeof(jint) == 4, "jint must be int32_t or uint32_t");
                env->SetObjectField(
                        java_image, data_int,
                        JInt(env).convert_array(reinterpret_cast<const uint32_t*>(object.data), N));
                break;
            case SEETA_AIP_VALUE_FLOAT32:
                env->SetObjectField(
                        java_image, data_float,
                        JFloat(env).convert_array(reinterpret_cast<const float*>(object.data), N));
                break;
        }

        return java_image;
    }

    AutoJObject convert(const NativeObject &object) const final {
        return convert(SeetaAIPImageData(object));
    }

    using JConverter::convert_array;

    AutoJObject convert_array(const SeetaAIPImageData *array, jsize N) const {
        AutoJObject result(env, env->NewObjectArray(N, clazz, nullptr));
        for (jsize i = 0; i < N; ++i) {
            env->SetObjectArrayElement(result.get<jobjectArray>(), i, convert(array[i]));
        }
        return result;
    }

    NativeObject convert(jobject object) const final {
        if (!object) return NativeObject();

        JImageFormat clazz_image_format(env);

        auto java_format = env->GetObjectField(object, format);
        defer(&JNIEnv::DeleteLocalRef, env, java_format);
        auto java_type = env->GetObjectField(object, type);
        defer(&JNIEnv::DeleteLocalRef, env, java_type);
        auto java_number = env->GetIntField(object, number);
        auto java_height = env->GetIntField(object, height);
        auto java_width = env->GetIntField(object, width);
        auto java_channels = env->GetIntField(object, channels);

        auto native_format = clazz_image_format.convert(java_format);

        auto N = jsize(java_number * java_height * java_width * java_channels);

        auto result = NativeObject(SEETA_AIP_IMAGE_FORMAT(native_format),
                                   uint32_t(java_number),
                                   uint32_t(java_width),
                                   uint32_t(java_height),
                                   uint32_t(java_channels));
        auto native_type = result.type();

        switch (native_type) {
            default:
                break;
            case SEETA_AIP_VALUE_BYTE: {
                static_assert(sizeof(jbyte) == 1, "jbyte must be int8_t or uint8_t");
                auto java_data = reinterpret_cast<jbyteArray>(env->GetObjectField(object, data_byte));
                defer_delete_local_ref(env, java_data);
                env->GetByteArrayRegion(java_data, 0, N, result.data<jbyte>());
                break;
            }
            case SEETA_AIP_VALUE_INT32: {
                static_assert(sizeof(jint) == 4, "jint must be int32_t or uint32_t");
                auto java_data = reinterpret_cast<jintArray>(env->GetObjectField(object, data_int));
                defer_delete_local_ref(env, java_data);
                env->GetIntArrayRegion(java_data, 0, N, result.data<jint>());
                break;
            }
            case SEETA_AIP_VALUE_FLOAT32: {
                auto java_data = reinterpret_cast<jfloatArray>(env->GetObjectField(object, data_float));
                defer_delete_local_ref(env, java_data);
                env->GetFloatArrayRegion(java_data, 0, N, result.data<jfloat>());
                break;
            }
        }

        return result;
    }
};

class JResult : public JConverter<seeta::aip::Instance::Result> {
public:
    jfieldID images;
    jfieldID objects;

    explicit JResult(JNIEnv *env)
            : JConverter(env, JNI_PACKAGE "Result") {
        images = env->GetFieldID(clazz, "images", "[L" JNI_PACKAGE "ImageData;");
        objects = env->GetFieldID(clazz, "objects", "[L" JNI_PACKAGE "Object;");
    }

    AutoJObject convert(const NativeObject &object) const final {
        JImageData clazz_image(env);
        JAIPObject clazz_object(env);

        auto java_images = clazz_image.convert_array(object.images.data, jsize(object.images.size));
        auto java_objects = clazz_object.convert_array(object.objects.data, jsize(object.objects.size));

        auto java_result = AutoJObject(env, construct());
        env->SetObjectField(java_result, images, java_images);
        env->SetObjectField(java_result, objects, java_objects);

        return java_result;
    }

    NativeObject convert(jobject object) const final {
        throw seeta::aip::Exception("Not implement convert seeta.aip.Result");
    }
};

