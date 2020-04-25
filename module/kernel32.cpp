//
// Created by SeetTech on 2020/4/25.
//

#include "seeta_aip_dll.h"
#include "seeta_aip.h"

#if SEETA_AIP_OS_WINDOWS
#pragma message("[WRANING] Using system kernel32.dll instead in windows.")
#else

SEETA_AIP_API extern "C" int *LoadLibrary(const char *libname) {
    return reinterpret_cast<int *>(seeta::aip::dlopen_v2(libname));
}

SEETA_AIP_API extern "C" int *GetProcAddress(int *lib, const char *symbol) {
    return reinterpret_cast<int *>(seeta::aip::dlsym(lib, symbol));
}

SEETA_AIP_API extern "C" int FreeLibrary(int *lib) {
    seeta::aip::dlclose(lib);
    return 1;
}

#endif
