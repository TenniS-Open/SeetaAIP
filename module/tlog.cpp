//
// Created by kier on 2020/4/18.
//

#include "seeta_aip_package.h"
#include "seeta_aip_shape.h"

#include "seeta_aip_struct.h"

#include <iostream>
#include <map>
#include <set>

#if defined(__ANDROID__)
#include "android/log.h"
#define ANDROID_LOG(_LEVEL, _TAG, ...) __android_log_print(_LEVEL, _TAG, __VA_ARGS__)
#endif

namespace log {
    class EndLine {};

    class Logger {
    public:
        std::ostringstream oss;
    };

    template <typename T>
    inline Logger &operator<<(Logger &s, const T &t) {
        s.oss << t;
        return s;
    }

    inline Logger &operator<<(Logger &s, const EndLine &) {
        const auto content = s.oss.str();
        s.oss << std::endl;
        const auto content_endl = s.oss.str();
        std::cout << content_endl;
        std::cout.flush();

#if defined(__ANDROID__)
        ANDROID_LOG(ANDROID_LOG_INFO, "AIPLOG", "%s", content.c_str());
#endif

        s.oss.clear();
        return s;
    }

    static Logger cout;
    static EndLine endl;
}


inline std::ostream &operator<<(std::ostream &out, const seeta::aip::Tensor &obj) {
    if (obj.empty()) {
        out << "null";
        return out;
    }
    out << "Tensor{shape=[";
    bool comma = false;
    for (auto &dim : obj.dims()) {
        if (comma) {
            out << ", ";
        } else {
            comma = true;
        }
        out << dim;
    }
    out << "], ";
    if (obj.type() == SEETA_AIP_VALUE_CHAR) {
        std::string tmp(obj.data<char>(), obj.bytes());
        out << "data=\"" << tmp << "\"";
    } else {
        out << "data[0]=...";
    }
    out << "}";
    return out;
}

inline std::ostream &operator<<(std::ostream &out, const SeetaAIPObject::Extra &obj) {
    seeta::aip::Tensor tmp;
    tmp.raw(obj);
    return out << tmp;
}

inline std::ostream &operator<<(std::ostream &out, const seeta::aip::Object &obj) {
    out << "{shape=" << obj.shape() << ", tags=[";
    bool comma = false;
    for (auto &tag : obj.tags()) {
        if (comma) {
            out << ", ";
        } else {
            comma = true;
        }
        out << "(" << tag.label << ", " << tag.score << ")";
    }
    out << "], extra=" << obj.extra() << "}";
    return out;
}

inline std::ostream &operator<<(std::ostream &out, const SeetaAIPObject &obj) {
    seeta::aip::Object tmp;
    tmp.raw(obj);
    return out << tmp;
}

inline std::ostream &operator<<(std::ostream &out, const seeta::aip::ImageData &obj) {
    out << "Image{NHWC=["
        << obj.number() << "," << obj.height() << ","
        << obj.width() << "," << obj.channels() << "], data[0]=...}";
    return out;
}

inline std::ostream &operator<<(std::ostream &out, const SeetaAIPImageData &obj) {
    seeta::aip::ImageData tmp;
    tmp.raw(obj);
    return out << tmp;
}

class MyPackage : public seeta::aip::Package {
public:
    using self = MyPackage;

    seeta::aip::Object m_empty_object;

    std::map<std::string, double> m_property_values;
    std::map<std::string, seeta::aip::Object> m_property_objects;
    std::vector<const char *> m_properties;

    log::Logger term_log;

    MyPackage() {
        term_log << "[aip] [" << this << "] constructed." << log::endl;
    }

    ~MyPackage() override {
        term_log << "[aip] [" << this << "] destructed." << log::endl;
    }

    void create(const SeetaAIPDevice &device,
            const std::vector<std::string> &models,
            const std::vector<SeetaAIPObject> &objects) override {
        term_log << "[aip] [" << this << "] create called." << log::endl;
        term_log << "[aip]     " << "param: device=" << "[" << device.device << ":" << device.id << "]" << log::endl;
        term_log << "[aip]     " << "param: models=" << "[";
        if (models.empty()) {
            term_log << "]" << log::endl;
        } else {
            term_log << log::endl;
            for (auto &model : models) {
                term_log << "[aip]         \"" << model << "\", " << log::endl;
            }
            term_log << "[aip]     " << "]" << log::endl;
        }
        term_log << "[aip]     " << "param: objects=" << "[";
        if (objects.empty()) {
            term_log << "]" << log::endl;
        } else {
            term_log << log::endl;
            for (auto &object : objects) {
                term_log << "[aip]         " << object << ", " << log::endl;
            }
            term_log << "[aip]     " << "]" << log::endl;
        }
    }

    void free() override {
        term_log << "[aip] [" << this << "] free called." << log::endl;
    }

    const char *error(int32_t errcode) override  {
        static const char *msg = "Error example.";
        term_log << "[aip] [" << this << "] error called." << log::endl;
        term_log << "[aip]     " << "param: errcode=" << errcode << log::endl;
        term_log << "[aip]     " << "return: \"" << msg << "\"" << log::endl;
        return msg;
    }

    const char **property() override {
        term_log << "[aip] [" << this << "] property called." << log::endl;

        m_properties.clear();
        std::set<std::string> cache;
        for (auto &pair : m_property_values) {
            auto &name = pair.first;
            if (cache.find(name) != cache.end()) continue;
            cache.insert(name);
            m_properties.push_back(name.c_str());
        }
        for (auto &pair : m_property_objects) {
            auto &name = pair.first;
            if (cache.find(name) != cache.end()) continue;
            cache.insert(name);
            m_properties.push_back(name.c_str());
        }

        term_log << "[aip]     " << "return: " << "[";
        if (m_properties.empty()) {
            term_log << "]" << log::endl;
        } else {
            term_log << log::endl;
            for (auto &property : m_properties) {
                term_log << "[aip]         \"" << property << "\", " << log::endl;
            }
            term_log << "[aip]     " << "]" << log::endl;
        }

        m_properties.push_back(nullptr);
        return m_properties.data();
    }

    void setd(const std::string &name, double value) override {
        term_log << "[aip] [" << this << "] setd called." << log::endl;
        term_log << "[aip]     " << "param: name=" << name << log::endl;
        term_log << "[aip]     " << "param: value=" << value << log::endl;
        m_property_values[name] = value;
    }

    double getd(const std::string &name) override {
        term_log << "[aip] [" << this << "] getd called." << log::endl;
        term_log << "[aip]     " << "param: name=" << name << log::endl;
        auto it = m_property_values.find(name);
        double value = 0;
        if (it != m_property_values.end()) {
            value = it->second;
        } else {
            term_log << "[aip]     " << "exception: property not set." << log::endl;
        }
        term_log << "[aip]     " << "return: " << value << log::endl;
        return value;
    }

    void set(const std::string &name, const SeetaAIPObject &value) override {
        term_log << "[aip] [" << this << "] set called." << log::endl;
        term_log << "[aip]     " << "param: name=" << name << log::endl;
        term_log << "[aip]     " << "param: value=" << value << log::endl;
        m_property_objects[name].raw(value);
    }

    SeetaAIPObject get(const std::string &name) override {
        term_log << "[aip] [" << this << "] get called." << log::endl;
        term_log << "[aip]     " << "param: name=" << name << log::endl;
        auto it = m_property_objects.find(name);
        SeetaAIPObject value = m_empty_object;
        if (it != m_property_objects.end()) {
            value = it->second;
        } else {
            term_log << "[aip]     " << "exception: property not set." << log::endl;
        }
        term_log << "[aip]     " << "return: " << value << log::endl;
        return value;
    }

    const char *tag(uint32_t method_id, uint32_t label_index, int32_t label_value) override {
        static const char *tag = "tag";
        term_log << "[aip] [" << this << "] tag called." << log::endl;
        term_log << "[aip]     " << "param: method_id=" << method_id << log::endl;
        term_log << "[aip]     " << "param: label_index=" << label_index << log::endl;
        term_log << "[aip]     " << "param: label_value=" << label_value << log::endl;
        term_log << "[aip]     " << "return: " << tag << log::endl;
        return tag;
    }

    void reset() override {
        term_log << "[aip] [" << this << "] reset called." << log::endl;
    }

    void forward(
            uint32_t method_id,
            const std::vector<SeetaAIPImageData> &images,
            const std::vector<SeetaAIPObject> &objects) override {
        term_log << "[aip] [" << this << "] forward called." << log::endl;
        term_log << "[aip]     " << "param: method_id=" << method_id << log::endl;
        term_log << "[aip]     " << "param: images=" << "[";
        if (images.empty()) {
            term_log << "]" << log::endl;
        } else {
            term_log << log::endl;
            for (auto &image : images) {
                term_log << "[aip]         " << image << ", " << log::endl;
            }
            term_log << "[aip]     " << "]" << log::endl;
        }
        term_log << "[aip]     " << "param: objects=" << "[";
        if (objects.empty()) {
            term_log << "]" << log::endl;
        } else {
            term_log << log::endl;
            for (auto &object : objects) {
                term_log << "[aip]         " << object << ", " << log::endl;
            }
            term_log << "[aip]     " << "]" << log::endl;
        }
        term_log << "[aip]     " << "return: images=[]" << log::endl;
        term_log << "[aip]     " << "return: objects=[]" << log::endl;
        result = Result();
    }
};

class Init {
public:
    Init() {
        log::cout << "[dll] [" << this << "] initialized." << log::endl;
    }

    ~Init() {
        log::cout << "[dll] [" << this << "] finalized." << log::endl;
    }
};

Init init;

int32_t seeta_aip_load(struct SeetaAIP *aip, uint32_t size) {
    log::cout << "[aip] seeta_aip_load called." << log::endl;
    log::cout << "[aip]     " << "param: aip=@" << aip << log::endl;
    log::cout << "[aip]     " << "param: size=" << size << log::endl;

    CHECK_AIP_SIZE(aip, size)
    static const char *support[] = {"cpu", "gpu", nullptr};
    seeta::aip::setup_aip_header(*aip,
            "Log",
            "Log every call",
            "log",
            "v1",
            "0.0.1",
            support);
    seeta::aip::setup_aip_entry<MyPackage>(*aip);

    log::cout << "[aip]     " << "return: 0" << log::endl;
    return 0;
}