//
// Created by SeetaTech on 2020/4/21.
//

#ifndef _INC_SEETA_AIP_DLL_H
#define _INC_SEETA_AIP_DLL_H

#include "seeta_aip_platform.h"
#include <string>

#if SEETA_AIP_OS_UNIX

#include <dlfcn.h>

#elif SEETA_AIP_OS_WINDOWS

#include <Windows.h>
#include <sstream>

#else

#pragma message("[WRANING] Using system not support dynamic library loading!")

#endif

namespace seeta {
    namespace aip {
        inline void *dlopen(const char *libname) {
#if SEETA_AIP_OS_UNIX
            auto handle = ::dlopen(libname, RTLD_LAZY | RTLD_LOCAL);
            return handle;
#elif SEETA_AIP_OS_WINDOWS
            auto instance = ::LoadLibraryA(libname);
            return static_cast<void*>(instance);
#else
            (void)(libname);
            return nullptr;
#endif
        }

        void *dlsym(void *handle, const char *symbol) {
#if SEETA_AIP_OS_UNIX
            return ::dlsym(handle, symbol);
#elif SEETA_AIP_OS_WINDOWS
            auto instance = static_cast<HINSTANCE>(handle);
            return ::GetProcAddressA(instance, symbol);
#else
            (void)(handle);
            (void)(symbol);
            return nullptr;
#endif
        }

        void dlclose(void *handle) {
#if SEETA_AIP_OS_UNIX
            ::dlclose(handle);
#elif SEETA_AIP_OS_WINDOWS
            auto instance = static_cast<HINSTANCE>(handle);
            ::FreeLibrary(instance);
#else
            (void)(handle);
#endif
        }

#if SEETA_AIP_OS_WINDOWS
        inline std::string format_message(DWORD dw) {
            LPVOID lpMsgBuf = nullptr;
            FormatMessageA(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                    NULL,
                    dw,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPTSTR) & lpMsgBuf,
                    0, NULL);
            std::ostringstream msg;
            msg << std::hex << "0x" << dw << ": ";
            if (lpMsgBuf != nullptr) {
                msg << lpMsgBuf;
                LocalFree(lpMsgBuf);
            }
            return msg.str();
        }
#endif

        std::string dlerror() {
#if SEETA_AIP_OS_UNIX
            auto error_message = ::dlerror();
            if (error_message) return error_message;
            return "No error.";
#elif SEETA_AIP_OS_WINDOWS
            return format_message(::GetLastError());
#else
            return "Undefined.";
#endif
        }
    }
}

#endif //_INC_SEETA_AIP_DLL_H
