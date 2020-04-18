//
// Created by kier on 2020/4/18.
//

#ifndef _INC_SEETA_AIP_PACKAGE_H
#define _INC_SEETA_AIP_PACKAGE_H


#include "seeta_aip.h"

#include <vector>
#include <string>

namespace seeta {
    namespace aip {
        class Package {
        public:
            using self = Package;

            virtual self *create(const SeetaAIPDevice &device, const std::vector<std::string> &models) = 0;
            virtual void free(self *obj) = 0;
        };
    }
}

#endif //_INC_SEETA_AIP_PACKAGE_H
