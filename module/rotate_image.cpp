//
// Created by kier on 2020/6/28.
//

#include "seeta_aip_package_v2.h"
#include "seeta_aip_shape.h"

#include <iostream>

class MyPackage : public seeta::aip::PackageV2 {
public:
    using self = MyPackage;

    int angle = 90;

    MyPackage() {
        bind_property("angle", angle,
                [](int a){ return a == 0 || a == 90 || a == 180 || a == 270; },
                "angle must be in {0, 90, 180, 270}");
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

    void forward_0(const std::vector<SeetaAIPImageData> &images,
                   const std::vector<SeetaAIPObject> &objects) {
        if (images.size() != 1) throw seeta::aip::Exception(SEETA_AIP_ERROR_MISMATCH_REQUIRED_INPUT_IMAGE);
        auto &image = images[0];
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

class Init {
public:
    Init() {
        std::cout << "[dll] initialized." << std::endl;
    }

    ~Init() {
        std::cout << "[dll] finalized." << std::endl;
    }
};

Init init;

int32_t seeta_aip_load(struct SeetaAIP *aip, uint32_t size) {
    std::cout << "[dll] package information loaded." << std::endl;
    CHECK_AIP_SIZE(aip, size)
    static const char *support[] = {"cpu", "gpu", nullptr};
    seeta::aip::setup_aip_header(*aip,
                                 "MyPackage",
                                 "Test AI Package",
                                 "Test",
                                 "v1",
                                 "0.0.1",
                                 support);
    seeta::aip::setup_aip_entry<MyPackage>(*aip);
    return 0;
}