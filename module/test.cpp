//
// Created by kier on 2020/4/18.
//

#include "seeta_aip_package.h"
#include "seeta_aip_shape.h"

#include <iostream>

class MyPackage : public seeta::aip::Package {
public:
    using self = MyPackage;

    const char *error(int32_t errcode) override {
        std::cout << "[aip] error" << std::endl;
        return nullptr;
    }

    void create(const SeetaAIPDevice &device,
            const std::vector<std::string> &models,
            const std::vector<SeetaAIPObject> &objects) override {
        std::cout << "[aip] created on ";
        std::cout << "[" << device.device << ":" << device.id << "] with: ";
        for (auto &model : models) {
            std::cout << model << ", ";
        }
        std::cout << std::endl;
    }

    void free() override {
        std::cout << "[aip] free" << std::endl;
    }

    std::vector<int32_t> property() override {
        return {3001, 0};
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
        std::cout << "[aip] forawrd image 0: [" << images[0].number << ", " << images[0].height << ", " << images[0].width << ", " << images[0].channels << "]" << std::endl;
        std::cout << "[aip] image 0: data(0) = " << int(reinterpret_cast<char *>(images[0].data)[0]) << std::endl;
        result.objects.resize(1);
        result.objects[0].tag(1, 0);
        result.objects[0].extra(seeta::aip::Tensor(SEETA_AIP_VALUE_FLOAT, {1}));
        result.objects[0].rextra().data<float>()[0] = 233;
    }

    const char *tag(uint32_t method_id, uint32_t label_index, int32_t label_value) override  {
        return "label";
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