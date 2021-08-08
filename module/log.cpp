//
// Created by kier on 2020/4/18.
//

#include "seeta_aip_package.h"
#include "seeta_aip_shape.h"

#include "seeta_aip_struct.h"

#include <iostream>
#include <map>
#include <set>


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

    MyPackage() {
        std::cout << "[aip] [" << this << "] constructed." << std::endl;
    }

    ~MyPackage() override {
        std::cout << "[aip] [" << this << "] destructed." << std::endl;
    }

    void create(const SeetaAIPDevice &device,
            const std::vector<std::string> &models,
            const std::vector<SeetaAIPObject> &objects) override {
        std::cout << "[aip] [" << this << "] create called." << std::endl;
        std::cout << "[aip]     " << "param: device=" << "[" << device.device << ":" << device.id << "]" << std::endl;
        std::cout << "[aip]     " << "param: models=" << "[";
        if (models.empty()) {
            std::cout << "]" << std::endl;
        } else {
            std::cout << std::endl;
            for (auto &model : models) {
                std::cout << "[aip]         \"" << model << "\", " << std::endl;
            }
            std::cout << "[aip]     " << "]" << std::endl;
        }
        std::cout << "[aip]     " << "param: objects=" << "[";
        if (objects.empty()) {
            std::cout << "]" << std::endl;
        } else {
            std::cout << std::endl;
            for (auto &object : objects) {
                std::cout << "[aip]         " << object << ", " << std::endl;
            }
            std::cout << "[aip]     " << "]" << std::endl;
        }
    }

    void free() override {
        std::cout << "[aip] [" << this << "] free called." << std::endl;
    }

    const char *error(int32_t errcode) override  {
        static const char *msg = "Error example.";
        std::cout << "[aip] [" << this << "] error called." << std::endl;
        std::cout << "[aip]     " << "param: errcode=" << errcode << std::endl;
        std::cout << "[aip]     " << "return: \"" << msg << "\"" << std::endl;
        return msg;
    }

    const char **property() override {
        std::cout << "[aip] [" << this << "] property called." << std::endl;

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

        std::cout << "[aip]     " << "return: " << "[";
        if (m_properties.empty()) {
            std::cout << "]" << std::endl;
        } else {
            std::cout << std::endl;
            for (auto &property : m_properties) {
                std::cout << "[aip]         \"" << property << "\", " << std::endl;
            }
            std::cout << "[aip]     " << "]" << std::endl;
        }

        m_properties.push_back(nullptr);
        return m_properties.data();
    }

    void setd(const std::string &name, double value) override {
        std::cout << "[aip] [" << this << "] setd called." << std::endl;
        std::cout << "[aip]     " << "param: name=" << name << std::endl;
        std::cout << "[aip]     " << "param: value=" << value << std::endl;
        m_property_values[name] = value;
    }

    double getd(const std::string &name) override {
        std::cout << "[aip] [" << this << "] getd called." << std::endl;
        std::cout << "[aip]     " << "param: name=" << name << std::endl;
        auto it = m_property_values.find(name);
        double value = 0;
        if (it != m_property_values.end()) {
            value = it->second;
        } else {
            std::cout << "[aip]     " << "exception: property not set." << std::endl;
        }
        std::cout << "[aip]     " << "return: " << value << std::endl;
        return value;
    }

    void set(const std::string &name, const SeetaAIPObject &value) override {
        std::cout << "[aip] [" << this << "] set called." << std::endl;
        std::cout << "[aip]     " << "param: name=" << name << std::endl;
        std::cout << "[aip]     " << "param: value=" << value << std::endl;
        m_property_objects[name].raw(value);
    }

    SeetaAIPObject get(const std::string &name) override {
        std::cout << "[aip] [" << this << "] get called." << std::endl;
        std::cout << "[aip]     " << "param: name=" << name << std::endl;
        auto it = m_property_objects.find(name);
        SeetaAIPObject value = m_empty_object;
        if (it != m_property_objects.end()) {
            value = it->second;
        } else {
            std::cout << "[aip]     " << "exception: property not set." << std::endl;
        }
        std::cout << "[aip]     " << "return: " << value << std::endl;
        return value;
    }

    const char *tag(uint32_t method_id, uint32_t label_index, int32_t label_value) override {
        static const char *tag = "tag";
        std::cout << "[aip] [" << this << "] tag called." << std::endl;
        std::cout << "[aip]     " << "param: method_id=" << method_id << std::endl;
        std::cout << "[aip]     " << "param: label_index=" << label_index << std::endl;
        std::cout << "[aip]     " << "param: label_value=" << label_value << std::endl;
        std::cout << "[aip]     " << "return: " << tag << std::endl;
        return tag;
    }

    void reset() override {
        std::cout << "[aip] [" << this << "] reset called." << std::endl;
    }

    void forward(
            uint32_t method_id,
            const std::vector<SeetaAIPImageData> &images,
            const std::vector<SeetaAIPObject> &objects) override {
        std::cout << "[aip] [" << this << "] forward called." << std::endl;
        std::cout << "[aip]     " << "param: method_id=" << method_id << std::endl;
        std::cout << "[aip]     " << "param: images=" << "[";
        if (images.empty()) {
            std::cout << "]" << std::endl;
        } else {
            std::cout << std::endl;
            for (auto &image : images) {
                std::cout << "[aip]         " << image << ", " << std::endl;
            }
            std::cout << "[aip]     " << "]" << std::endl;
        }
        std::cout << "[aip]     " << "param: objects=" << "[";
        if (objects.empty()) {
            std::cout << "]" << std::endl;
        } else {
            std::cout << std::endl;
            for (auto &object : objects) {
                std::cout << "[aip]         " << object << ", " << std::endl;
            }
            std::cout << "[aip]     " << "]" << std::endl;
        }
        std::cout << "[aip]     " << "return: images=[]" << std::endl;
        std::cout << "[aip]     " << "return: objects=[]" << std::endl;
        result = Result();
    }
};

class Init {
public:
    Init() {
        std::cout << "[dll] [" << this << "] initialized." << std::endl;
    }

    ~Init() {
        std::cout << "[dll] [" << this << "] finalized." << std::endl;
    }
};

Init init;

int32_t seeta_aip_load(struct SeetaAIP *aip, uint32_t size) {
    std::cout << "[aip] seeta_aip_load called." << std::endl;
    std::cout << "[aip]     " << "param: aip=@" << aip << std::endl;
    std::cout << "[aip]     " << "param: size=" << size << std::endl;

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

    std::cout << "[aip]     " << "return: 0" << std::endl;
    return 0;
}