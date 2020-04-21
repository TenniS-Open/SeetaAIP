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

namespace seeta {
    namespace aip {
        class Exception : std::exception {
        public:
            int32_t errcode() const {
                return m_errcode;
            }
            const std::string &message() const {
                return m_message;
            }

        private:
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
            using RawPtr = T *;

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

            operator RawPtr() { return this->raw(); }

            operator const RawPtr() const { return this->raw(); }

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
                    case SEETA_AIP_VALUE_BYTE:
                        return 1;
                    case SEETA_AIP_VALUE_FLOAT:
                        return 4;
                    case SEETA_AIP_VALUE_INT:
                        return 4;
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

            const Shape &shape() const { return m_shape; }

            self &shape(const Shape &val) {
                m_shape = val;
                return *this;
            }

            const Tensor &extra() const { return m_extra; }

            self &extra(const Tensor &val) {
                m_extra = val;
                return *this;
            }

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

            ImageData(SEETA_AIP_VALUE_TYPE type,
                      uint32_t number,
                      uint32_t width,
                      uint32_t height,
                      uint32_t channels,
                      void *data = nullptr) {
                this->m_raw.type = int32_t(type);
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

            ImageData(SEETA_AIP_VALUE_TYPE type,
                      uint32_t width,
                      uint32_t height,
                      uint32_t channels,
                      void *data = nullptr)
                      : self(type, 1, width, height, channels, data) {
            }

            _SEETA_AIP_WRAPPER_DECLARE_GETTER(type, SEETA_AIP_VALUE_TYPE)

            _SEETA_AIP_WRAPPER_DECLARE_GETTER(number, uint32_t)

            _SEETA_AIP_WRAPPER_DECLARE_GETTER(height, uint32_t)

            _SEETA_AIP_WRAPPER_DECLARE_GETTER(width, uint32_t)

            _SEETA_AIP_WRAPPER_DECLARE_GETTER(channels, uint32_t)

            uint32_t element_width() const {
                switch (type()) {
                    case SEETA_AIP_VALUE_BYTE:
                        return 1;
                    case SEETA_AIP_VALUE_FLOAT:
                        return 4;
                    case SEETA_AIP_VALUE_INT:
                        return 4;
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
                auto bytes = this->bytes();
                m_data.reset(new char[bytes], std::default_delete<char[]>());
                std::memcpy(m_data.get(), m_raw.data, bytes);
            }

        private:
            std::shared_ptr<char> m_data;
        };

        class Device : public Wrapper<SeetaAIPDevice> {
        public:
            using self = Device;

            Device()
                : self("cpu") {}

            explicit Device(const std::string &device, int32_t id = 0) {
                this->device(device);
                this->id(id);
            }

            explicit Device(const char *device, int32_t id = 0) {
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
