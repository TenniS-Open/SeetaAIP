//
// Created by kier on 2020/6/28.
//

#include "seeta_aip_package_v2.h"
#include "seeta_aip_shape.h"

#include <deque>
#include <iostream>

class MyPackage : public seeta::aip::PackageV2 {
public:
    using self = MyPackage;

    std::deque<Result> m_queue;
    int m_size = 1;

    MyPackage() {
        bind_property("size", m_size,
                [](int a) { return a >= 1; },
                "size must greater than 0");
    }

    void create(const SeetaAIPDevice &device,
                const std::vector<std::string> &models,
                const std::vector<SeetaAIPObject> &objects) override {
        if (models.size() != 0) throw seeta::aip::Exception(SEETA_AIP_ERROR_MODEL_MISMATCH);
    }

    void free() override {
    }

    void reset() override {
        this->result = Result();
    }

    void copy(Result &result,
              const std::vector<SeetaAIPImageData> &images,
              const std::vector<SeetaAIPObject> &objects) {
        result.images.clear();
        for (auto &image : images) {
            seeta::aip::ImageData tmp;
            tmp.raw(image);
            result.images.emplace_back(std::move(tmp));
        }
        for (auto &object : objects) {
            seeta::aip::Object tmp;
            tmp.raw(object);
            result.objects.emplace_back(std::move(tmp));
        }
    }

    void forward_0(const std::vector<SeetaAIPImageData> &images,
                   const std::vector<SeetaAIPObject> &objects) {
        Result result;
        copy(result, images, objects);
        m_queue.emplace_back(result);
        if (m_queue.size() > m_size) {
            this->result = m_queue.front();
            m_queue.pop_front();
        } else {
            this->result = Result();
        }
    }

    void forward(
            uint32_t method_id,
            const std::vector<SeetaAIPImageData> &images,
            const std::vector<SeetaAIPObject> &objects) override {
        switch (method_id) {
            default:
                throw seeta::aip::Exception(SEETA_AIP_ERROR_METHOD_ID_OUT_OF_RANGE);
            case 0:
                forward_0(images, objects);
                break;
        }
    }
};

int32_t seeta_aip_load(struct SeetaAIP *aip, uint32_t size) {
    CHECK_AIP_SIZE(aip, size)
    static const char *support[] = {"cpu", "gpu", nullptr};
    seeta::aip::setup_aip_header(*aip,
                                 "Delay",
                                 "delay output images and objects by inputs, delay size frame",
                                 "delay",
                                 "v1",
                                 "0.0.1",
                                 support);
    seeta::aip::setup_aip_entry<MyPackage>(*aip);
    return 0;
}