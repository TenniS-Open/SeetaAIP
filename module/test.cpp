//
// Created by kier on 2020/4/18.
//

#include "seeta_aip_package_v2.h"
#include "seeta_aip_shape.h"

#include <iostream>

class MyPackage : public seeta::aip::PackageV2 {
public:
    using self = MyPackage;

    int min_face_size = 122;
    seeta::aip::Object max_face_size;

    MyPackage() {
        bind_error(1001, "Error");
        bind_property("min_face_size", min_face_size);
        bind_property("max_face_size", max_face_size);
        bind_tag(0, 0, 0, "face");
        bind_tag(0, 0, {{1, "hand"}, {2, "body"}});
        bind_tag(0, {{1, 1, "hand"}, {1, 3, "body"}});
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
        result.objects[0].extra(seeta::aip::Tensor(SEETA_AIP_VALUE_FLOAT32, {1}));
        result.objects[0].rextra().data<float>()[0] = 233;
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