//
// Created by kier on 2020/6/28.
//

#include "seeta_aip_package_v2.h"
#include "seeta_aip_shape.h"

#include <iostream>

class MyPackage : public seeta::aip::PackageV2 {
public:
    using self = MyPackage;

    bool reverse = false;

    MyPackage() {
        bind_property("reverse", reverse);
    }

    void create(const SeetaAIPDevice &device,
                const std::vector<std::string> &models,
                const std::vector<SeetaAIPObject> &objects) override {
        if (models.size() != 0) throw seeta::aip::Exception(SEETA_AIP_ERROR_MODEL_MISMATCH);
    }

    void free() override {
    }

    void reset() override {
    }

    void copy(const std::vector<SeetaAIPImageData> &images,
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

    void copy_reverse(const std::vector<SeetaAIPImageData> &images,
              const std::vector<SeetaAIPObject> &objects) {
        result.images.clear();
        for (auto it = images.rbegin(); it != images.rend(); ++it) {
            auto &image = *it;
            seeta::aip::ImageData tmp;
            tmp.raw(image);
            result.images.emplace_back(std::move(tmp));
        }
        result.objects.clear();
        for (auto it = objects.rbegin(); it != objects.rend(); ++it) {
            auto &object = *it;
            seeta::aip::Object tmp;
            tmp.raw(object);
            result.objects.emplace_back(std::move(tmp));
        }
    }

    void forward_0(const std::vector<SeetaAIPImageData> &images,
                   const std::vector<SeetaAIPObject> &objects) {
        if (reverse) {
            copy_reverse(images, objects);
        } else {
            copy(images, objects);
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
                                 "Copy",
                                 "copy input images and objects",
                                 "copy",
                                 "v1",
                                 "0.0.1",
                                 support);
    seeta::aip::setup_aip_entry<MyPackage>(*aip);
    return 0;
}