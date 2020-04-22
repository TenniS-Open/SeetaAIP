//
// Created by SeetaTech on 2020/4/22.
//

#include "seeta_aip_engine.h"

void test1() {
    using namespace seeta::aip;
    Engine engine("../lib/test");

    Instance instance(engine.aip(), "cpu", {"1"});
    instance.dispose();
}

void test2() {
    using namespace seeta::aip;
    auto engine = std::make_shared<Engine>("../lib/test");

    Instance instance(engine, "cpu", {"1"});
    instance.dispose();
}

void test3() {
    using namespace seeta::aip;

    Instance instance("../lib/test", "cpu", {"1"});
    instance.dispose();
}

int main() {
    test1();
    test2();
    test3();
    return 0;
}
