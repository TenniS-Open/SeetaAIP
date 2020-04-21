//
// Created by kier on 2020/4/18.
//

#include "seeta_aip_package.h"

#include <iostream>

class MyPackage : public seeta::aip::Package {
public:
    using self = MyPackage;

    const char *error(int32_t errcode) override {
        return nullptr;
    }

    void init(const SeetaAIPDevice &device, const std::vector<std::string> &models) override {

    }

    std::vector<int32_t> property() override {
        return {};
    }

    void set(int32_t property_id, double value) override {

    }

    double get(int32_t property_id) override {
        return 0;
    }

    void reset() override {

    }

    void forward(
            uint32_t method_id,
            const std::vector<SeetaAIPImageData> &images,
            const std::vector<SeetaAIPObject> &objects) override {
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
    static const char *support[] = {"cpu", nullptr};
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