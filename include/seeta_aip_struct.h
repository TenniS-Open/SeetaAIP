//
// Created by SeetaTech on 2020/4/18.
//

#ifndef _INC_SEETA_AIP_CPP_H
#define _INC_SEETA_AIP_CPP_H

#include "seeta_aip.h"

#include <vector>
#include <memory>
#include <numeric>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>
#include <iomanip>

#if defined(_MSC_VER)
#define SEETA_AIP_NOEXCEPT
#else
#define SEETA_AIP_NOEXCEPT noexcept
#endif

namespace seeta {
    namespace aip {
        class Exception : public std::exception {
        public:
            explicit Exception(int32_t errcode)
                    : m_what(Message(errcode))
                    , m_errcode(errcode) {}

            explicit Exception(const std::string &message)
                    : m_what(Message(message))
                    , m_message(message) {}

            explicit Exception(int32_t errcode, const std::string &message)
                    : m_what(Message(errcode, message))
                    , m_errcode(errcode)
                    , m_message(message) {}

            explicit Exception(const char *message) {
                if (message) {
                    m_what = Message(message);
                    m_message = message;
                }
            }

            explicit Exception(int32_t errcode, const char *message) {
                if (message) {
                    m_what = Message(errcode, message);
                    m_errcode = errcode;
                    m_message = message;
                } else {
                    m_what = Message(errcode);
                    m_errcode = errcode;
                }
            }

            const char *what() const SEETA_AIP_NOEXCEPT override {
                return m_what.c_str();
            }

            int32_t errcode() const {
                return m_errcode;
            }

            const std::string &message() const {
                return m_message;
            }

            static std::string Message(int32_t errcode) {
                std::ostringstream oss;
                oss << "(";
                oss << std::hex << "0x" << std::setw(8) << std::setfill('0') << errcode;
                oss << ")";
                return oss.str();
            }

            static std::string Message(int32_t errcode, const std::string &message) {
                std::ostringstream oss;
                oss << "(";
                oss << std::hex << "0x" << std::setw(8) << std::setfill('0') << errcode;
                oss << ")";
                oss << ": " << message;
                return oss.str();
            }

            static std::string Message(const std::string &message) {
                std::ostringstream oss;
                oss << message;
                return oss.str();
            }

        private:
            std::string m_what;
            int32_t m_errcode = -1;
            std::string m_message;
        };

        using Point = SeetaAIPPoint;

        inline std::ostream &operator<<(std::ostream &out, const Point &point) {
            std::ostringstream oss;

            oss << "(" << point.x << ", " << point.y << ")";

            return out << oss.str();
        }

        template<typename T>
        class Wrapper {
        public:
            using self = Wrapper;

            using Raw = T;

            virtual ~Wrapper() = default;

            void raw(const Raw &other) {
                m_raw = other;
                this->importer();
            }

            Raw *raw() {
                this->exporter();
                return &m_raw;
            }

            const Raw *raw() const {
                const_cast<self *>(this)->exporter();
                return &m_raw;
            }

            operator Raw() const { return *this->raw(); }

            operator Raw*() { return this->raw(); }

            operator const Raw*() const { return this->raw(); }

        protected:
            mutable Raw m_raw;

        private:
            virtual void exporter() {};

            virtual void importer() {};
        };

#define _SEETA_AIP_WRAPPER_DECLARE_ATTR(attr, dst) \
        dst attr() const { return dst(m_raw.attr); } \
        self &attr(dst val) { this->m_raw.attr = decltype(this->m_raw.attr)(val); return *this; }

#define _SEETA_AIP_WRAPPER_DECLARE_GETTER(attr, dst) \
        dst attr() const { return dst(m_raw.attr); }

        class ShapeType {
        public:
            static const char *String(SEETA_AIP_SHAPE_TYPE type) {
                switch (type) {
                    default:
                    case SEETA_AIP_UNKNOWN_SHAPE: return "Unknown";
                    case SEETA_AIP_POINTS: return "Points";
                    case SEETA_AIP_LINES: return "Lines";
                    case SEETA_AIP_RECTANGLE: return "Rectangle";
                    case SEETA_AIP_PARALLELOGRAM: return "Parallelogram";
                    case SEETA_AIP_POLYGON: return "Polygon";
                    case SEETA_AIP_CIRCLE: return "Circle";
                    case SEETA_AIP_CUBE: return "Cube";
                    case SEETA_AIP_NO_SHAPE: return "NoShape";
                }
            }
        };

        class Shape : public Wrapper<SeetaAIPShape> {
        public:
            using self = Shape;
            using supper = Wrapper<SeetaAIPShape>;

            using Landmarks = std::vector<Point>;

            Shape() {
                type(SEETA_AIP_UNKNOWN_SHAPE);
                scale(1);
                rotate(0);
            }

            _SEETA_AIP_WRAPPER_DECLARE_ATTR(type, SEETA_AIP_SHAPE_TYPE)

            _SEETA_AIP_WRAPPER_DECLARE_ATTR(scale, float)

            _SEETA_AIP_WRAPPER_DECLARE_ATTR(rotate, float)

            const std::vector<Point> &landmarks() const {
                return m_landmarks;
            }

            self &landmarks(const std::vector<Point> &val) {
                m_landmarks = val;
                return *this;
            }

            void append(const Point &point) {
                m_landmarks.emplace_back(point);
            }

            void exporter() override {
                m_raw.landmarks.data = m_landmarks.data();
                m_raw.landmarks.size = uint32_t(m_landmarks.size());
            }

            void importer() override {
                m_landmarks = Landmarks(m_raw.landmarks.data, m_raw.landmarks.data + m_raw.landmarks.size);
            }

            virtual std::string str() const {
                std::ostringstream oss;

                oss << ShapeType::String(this->type()) << "{points=";
                PlotPoints(oss, this->landmarks());
                oss << ", scale=" << this->scale();
                oss << ", rotate=" << this->rotate() << "}";

                return oss.str();
            }

            static std::ostream &PlotPoints(std::ostream &out, const Landmarks &landmarks) {
                out << "[";
                for (size_t i = 0; i < landmarks.size(); ++i) {
                    auto &point = landmarks[i];
                    if (i) out << ", ";
                    out << "(" << point.x << ", " << point.y << ")";
                }
                out << "]";
                return out;
            }

            inline friend std::ostream &operator<<(std::ostream &out, const self &shape) {
                return out << shape.str();
            }

        private:
            Landmarks m_landmarks;
        };

        class Tensor : public Wrapper<SeetaAIPObject::Extra> {
        public:
            using self = Tensor;

            using Dims = std::vector<uint32_t>;

            Tensor()
                    : self(SEETA_AIP_VALUE_BYTE, {}) {
            }

            Tensor(SEETA_AIP_VALUE_TYPE type, const std::vector<uint32_t> &dims) {
                m_raw.type = int32_t(type);
                m_dims = dims;
                auto bytes = this->bytes();
                m_data.reset(new char[bytes], std::default_delete<char[]>());
            }

            _SEETA_AIP_WRAPPER_DECLARE_GETTER(type, SEETA_AIP_VALUE_TYPE)

            uint32_t element_width() const {
                switch (type()) {
                    default:
                        return 0;
                    case SEETA_AIP_VALUE_BYTE:
                        return 1;
                    case SEETA_AIP_VALUE_FLOAT32:
                        return 4;
                    case SEETA_AIP_VALUE_INT32:
                        return 4;
                    case SEETA_AIP_VALUE_FLOAT64:
                        return 8;
                }
            }

            uint32_t bytes() const {
                return std::accumulate(m_dims.begin(), m_dims.end(), uint32_t(1)) * element_width();
            }

            void *data() { return m_data.get(); }

            const void *data() const { return m_data.get(); }

            template<typename T>
            T *data() { return reinterpret_cast<T *>(m_data.get()); }

            template<typename T>
            const T *data() const { return reinterpret_cast<const T *>(m_data.get()); }

            template<typename T>
            T &data(size_t i) { return this->data<T>()[i]; }

            template<typename T>
            const T &data(size_t i) const { return this->data<T>()[i]; }

            template<typename T>
            T &data(int i) { return this->data<T>()[i]; }

            template<typename T>
            const T &data(int i) const { return this->data<T>()[i]; }

            const Dims &dims() const { return m_dims; }

            void exporter() override {
                m_raw.dims.data = m_dims.data();
                m_raw.dims.size = uint32_t(m_dims.size());
                m_raw.data = m_data.get();
            }

            void importer() override {
                m_dims = Dims(m_raw.dims.data, m_raw.dims.data + m_raw.dims.size);
                auto bytes = this->bytes();
                m_data.reset(new char[bytes], std::default_delete<char[]>());
                std::memcpy(m_data.get(), m_raw.data, bytes);
            }

        private:
            std::shared_ptr<char> m_data;
            Dims m_dims;
        };

        class Object : public Wrapper<SeetaAIPObject> {
        public:
            using self = Object;

            using Tag = SeetaAIPObject::Tag;
            using Tags = std::vector<Tag>;

            Object() = default;

            Object(Shape shape)
                : m_shape(std::move(shape)) {}

            Object(Shape shape, Tags tags)
                : m_shape(std::move(shape)), m_tags(std::move(tags)) {}

            Object(Shape shape, Tags tags, Tensor extra)
                : m_shape(std::move(shape)), m_tags(std::move(tags)), m_extra(std::move(extra)) {}

            Object(Shape shape, Tensor extra)
                : m_shape(std::move(shape)), m_extra(std::move(extra)) {}

            Object(Tensor extra)
                : m_extra(std::move(extra)) {}

            Shape &rshape() { return m_shape; }

            const Shape &shape() const { return m_shape; }

            self &shape(const Shape &val) {
                m_shape = val;
                return *this;
            }

            Tensor &rextra() { return m_extra; }

            const Tensor &extra() const { return m_extra; }

            self &extra(const Tensor &val) {
                m_extra = val;
                return *this;
            }

            Tags &rtags() { return m_tags; };

            const Tags &tags() const { return m_tags; };

            void tags(const Tags &val) { m_tags = val; }

            void clear_tags() { m_tags.clear(); }

            self &tag(const Tag &val) {
                m_tags.push_back(val);
                return *this;
            }

            self &tag(int32_t label, float score) {
                m_tags.push_back({label, score});
                return *this;
            }

            self &tag(float score) { return tag(0, score); }

            void exporter() override {
                m_raw.shape = m_shape;
                m_raw.tags.data = m_tags.data();
                m_raw.tags.size = uint32_t(m_tags.size());
                m_raw.extra = m_extra;
            }

            void importer() override {
                m_shape.raw(m_raw.shape);
                m_tags = Tags(m_raw.tags.data, m_raw.tags.data + m_raw.tags.size);
                m_extra.raw(m_raw.extra);
            }

        private:
            Shape m_shape;
            Tags m_tags;
            Tensor m_extra;
        };

        class ImageData : public Wrapper<SeetaAIPImageData> {
        public:
            using self = ImageData;

            ImageData(SEETA_AIP_IMAGE_FORMAT format,
                      uint32_t number,
                      uint32_t width,
                      uint32_t height,
                      uint32_t channels,
                      void *data = nullptr) {
                auto type = GetType(format);
                this->m_type = type;
                this->m_raw.format = int32_t(format);
                this->m_raw.number = number;
                this->m_raw.width = width;
                this->m_raw.height = height;
                this->m_raw.channels = channels;
                auto bytes = this->bytes();
                m_data.reset(new char[bytes], std::default_delete<char[]>());
                if (data) {
                    std::memcpy(m_data.get(), data, bytes);
                }
            }

            ImageData(SEETA_AIP_IMAGE_FORMAT format,
                      uint32_t width,
                      uint32_t height,
                      uint32_t channels,
                      void *data = nullptr)
                      : self(format, 1, width, height, channels, data) {
            }

            static SEETA_AIP_VALUE_TYPE GetType(SEETA_AIP_IMAGE_FORMAT format) {
                switch (format) {
                    default:
                    case SEETA_AIP_FORMAT_U8RAW:
                    case SEETA_AIP_FORMAT_U8RGB:
                    case SEETA_AIP_FORMAT_U8BGR:
                    case SEETA_AIP_FORMAT_U8RGBA:
                    case SEETA_AIP_FORMAT_U8BGRA:
                    case SEETA_AIP_FORMAT_U8Y:
                        return SEETA_AIP_VALUE_BYTE;
                    case SEETA_AIP_FORMAT_F32RAW:
                        return SEETA_AIP_VALUE_FLOAT32;
                    case SEETA_AIP_FORMAT_I32RAW:
                        return SEETA_AIP_VALUE_INT32;
                }
            }

            SEETA_AIP_VALUE_TYPE type() const { return m_type; }

            _SEETA_AIP_WRAPPER_DECLARE_GETTER(format, SEETA_AIP_IMAGE_FORMAT)

            _SEETA_AIP_WRAPPER_DECLARE_GETTER(number, uint32_t)

            _SEETA_AIP_WRAPPER_DECLARE_GETTER(height, uint32_t)

            _SEETA_AIP_WRAPPER_DECLARE_GETTER(width, uint32_t)

            _SEETA_AIP_WRAPPER_DECLARE_GETTER(channels, uint32_t)

            uint32_t element_width() const {
                switch (type()) {
                    default:
                        return 0;
                    case SEETA_AIP_VALUE_BYTE:
                        return 1;
                    case SEETA_AIP_VALUE_FLOAT32:
                        return 4;
                    case SEETA_AIP_VALUE_INT32:
                        return 4;
                    case SEETA_AIP_VALUE_FLOAT64:
                        return 8;
                }
            }

            uint32_t bytes() const {
                return number() * height() * width() * channels() * element_width();
            }

            void *data() { return m_data.get(); }

            const void *data() const { return m_data.get(); }

            template<typename T>
            T *data() { return reinterpret_cast<T *>(m_data.get()); }

            template<typename T>
            const T *data() const { return reinterpret_cast<const T *>(m_data.get()); }

            template<typename T>
            T &data(size_t i) { return this->data<T>()[i]; }

            template<typename T>
            const T &data(size_t i) const { return this->data<T>()[i]; }

            template<typename T>
            T &data(int i) { return this->data<T>()[i]; }

            template<typename T>
            const T &data(int i) const { return this->data<T>()[i]; }

            std::vector<uint32_t> dims() const { return {number(), height(), width(), channels()}; }

            void exporter() override {
                m_raw.data = m_data.get();
            }

            void importer() override {
                m_type = GetType(SEETA_AIP_IMAGE_FORMAT(m_raw.format));
                auto bytes = this->bytes();
                m_data.reset(new char[bytes], std::default_delete<char[]>());
                std::memcpy(m_data.get(), m_raw.data, bytes);
            }

        private:
            std::shared_ptr<char> m_data;
            SEETA_AIP_VALUE_TYPE m_type;
        };

        class Device : public Wrapper<SeetaAIPDevice> {
        public:
            using self = Device;

            Device()
                : self("cpu") {}

            Device(const std::string &device, int32_t id = 0) {
                this->device(device);
                this->id(id);
            }

            Device(const char *device, int32_t id = 0) {
                this->device(device);
                this->id(id);
            }

            _SEETA_AIP_WRAPPER_DECLARE_ATTR(id, int32_t)

            const char *device() const { return m_device.get(); }

            self &device(const std::string &val) {
                m_raw.device = val.c_str();
                this->importer();
                return *this;
            }

            self &device(const char *val) {
                m_raw.device = val;
                this->importer();
                return *this;
            }

            void exporter() override {
                m_raw.device = m_device.get();
            }

            void importer() override {
                auto length = std::strlen(m_raw.device);
                m_device.reset(new char[length + 1], std::default_delete<char[]>());
                std::memcpy(m_device.get(), m_raw.device, length + 1);
            }

        private:
            std::shared_ptr<char> m_device;
        };
    }
}

#endif //_INC_SEETA_AIP_CPP_H
