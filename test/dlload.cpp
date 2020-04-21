//
// Created by SeetaTech on 2020/4/21.
//

#include "seeta_aip_dll.h"
#include "seeta_aip.h"

#include <iostream>

int main() {
    std::cout << "[Host] dlopen" << std::endl;
    auto handle = seeta::aip::dlopen_v2("../lib/test");
    std::cout << "[Host] dlopen " << handle << std::endl;

    if (handle == nullptr) {
        std::cout << "[Host] dlopen failed: " << seeta::aip::dlerror() << std::endl;
        return 1;
    }

    std::cout << "[Host] dlsym" << std::endl;
    auto func = reinterpret_cast<seeta_aip_load_entry*>(seeta::aip::dlsym(handle, "seeta_aip_load"));
    std::cout << "[Host] dlsym " << func << std::endl;

    SeetaAIP aip;
    func(&aip, sizeof(SeetaAIP));

    std::cout << "[Host] dlclose" << std::endl;
    seeta::aip::dlclose(handle);
    std::cout << "[Host] dlclose " << "done." << std::endl;
    return 0;
}

