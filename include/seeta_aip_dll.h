//
// Created by SeetaTech on 2020/4/21.
//

#ifndef _INC_SEETA_AIP_DLL_H
#define _INC_SEETA_AIP_DLL_H

#include "seeta_aip_platform.h"
#include <string>
#include <sstream>
#include <iomanip>

#if SEETA_AIP_OS_UNIX

#include <dlfcn.h>

#elif SEETA_AIP_OS_WINDOWS

#include <Windows.h>

#else

#pragma message("[WRANING] Using system not support dynamic library loading!")

#endif

namespace seeta {
    namespace aip {
        /**
         * Load dynamic library
         * @param [in] libname full path to library name
         * @return handle to library
         * @note call dlclose to close handle
         * @note return null if failed. Call dlerror to get error message.
         */
        inline void *dlopen(const char *libname) {
#if SEETA_AIP_OS_UNIX
            // ::setenv("OMP_WAIT_POLICY", "passive", 1);
            auto handle = ::dlopen(libname, RTLD_LAZY | RTLD_LOCAL);
            return handle;
#elif SEETA_AIP_OS_WINDOWS
            ::SetEnvironmentVariableA("OMP_WAIT_POLICY", "passive");
            auto instance = ::LoadLibraryA(libname);
            return static_cast<void*>(instance);
#else
            (void)(libname);
            return nullptr;
#endif
        }

        /**
         * Open symbol in dynamic library
         * @param [in] handle return value of dlopen
         * @param [in] symbol symbol name
         * @return pointer to function
         */
        inline void *dlsym(void *handle, const char *symbol) {
#if SEETA_AIP_OS_UNIX
            return ::dlsym(handle, symbol);
#elif SEETA_AIP_OS_WINDOWS
            auto instance = static_cast<HMODULE>(handle);
            return reinterpret_cast<void*>(::GetProcAddress(instance, symbol));
#else
            (void)(handle);
            (void)(symbol);
            return nullptr;
#endif
        }

        /**
         * Close dynamic library
         * @param [in] handle return value of dlopen
         */
        inline void dlclose(void *handle) {
#if SEETA_AIP_OS_UNIX
            ::dlclose(handle);
#elif SEETA_AIP_OS_WINDOWS
            // ::SetEnvironmentVariableA("OMP_WAIT_POLICY", "passive");
            auto instance = static_cast<HMODULE>(handle);
            ::FreeLibrary(instance);
#else
            (void)(handle);
#endif
        }

#if SEETA_AIP_OS_WINDOWS
        inline std::string _format_message(DWORD dw) {
            LPVOID lpMsgBuf = nullptr;
            ::FormatMessageA(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                    NULL,
                    dw,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPTSTR) & lpMsgBuf,
                    0, NULL);
            std::ostringstream msg;
            msg << std::hex << "0x" << std::setw(8) << std::setfill('0') << dw << ": ";
            if (lpMsgBuf != nullptr) {
                msg << std::string(reinterpret_cast<char *>(lpMsgBuf));
                LocalFree(lpMsgBuf);
            }
            return msg.str();
        }
#endif

        /**
         * Get last dlopen error message
         * @return error message
         */
        inline std::string dlerror() {
#if SEETA_AIP_OS_UNIX
            auto error_message = ::dlerror();
            if (error_message) return error_message;
            return "No error.";
#elif SEETA_AIP_OS_WINDOWS
            return _format_message(::GetLastError());
#else
            return "dlopen undefined.";
#endif
        }

        inline std::string _cut_path_tail(const std::string &path, std::string &tail) {
            auto win_sep_pos = path.rfind('\\');
            auto unix_sep_pos = path.rfind('/');
            auto sep_pos = win_sep_pos;
            if (sep_pos == std::string::npos) sep_pos = unix_sep_pos;
            else if (unix_sep_pos != std::string::npos && unix_sep_pos > sep_pos) sep_pos = unix_sep_pos;
            if (sep_pos == std::string::npos) {
                tail = path;
                return std::string();
            }
            tail = path.substr(sep_pos + 1);
            return path.substr(0, sep_pos);
        }

        inline char _file_separator() {
#if SEETA_AIP_OS_WINDOWS
            return '\\';
#else
            return '/';
#endif
        }

        /**
         * Load dynamic library
         * @param [in] libname path to libname, can ignore the the prefix or suffix of libname
         * @return handle to library
         * @note call dlclose to close handle
         * @note return null if failed. Call dlerror to get error message.
         * Example dlopen_v2("test") can load `test`, `libtest.so` or `test.so` on linux instead.
         */
        inline void *dlopen_v2(const std::string &libname) {
#if SEETA_AIP_OS_MAC || SEETA_AIP_OS_IOS
            static const char *prefix = "lib";
            static const char *suffix = ".dylib";
#elif SEETA_AIP_OS_UNIX
            static const char *prefix = "lib";
            static const char *suffix = ".so";
#elif SEETA_AIP_OS_WINDOWS
            static const char *prefix = "lib";
            static const char *suffix = ".dll";
#endif
            // first open library
            auto handle = dlopen(libname.c_str());
            if (handle) return handle;

            // second open library
            std::string tail;
            std::string head = _cut_path_tail(libname, tail);
            std::ostringstream fixed_libname_buf;
            if (head.empty()) {
                fixed_libname_buf << prefix << tail << suffix;
            } else {
                fixed_libname_buf << head << _file_separator() << prefix << tail << suffix;
            }
            auto fixed_libname = fixed_libname_buf.str();
            handle = dlopen(fixed_libname.c_str());
            if (handle) return handle;

            // third open library
            fixed_libname_buf.str("");
            if (head.empty()) {
                fixed_libname_buf << tail << suffix;
            } else {
                fixed_libname_buf << _file_separator() << tail << suffix;
            }
            fixed_libname = fixed_libname_buf.str();
            handle = dlopen(fixed_libname.c_str());
            if (handle) return handle;

            // 4-th open library
            handle = dlopen(libname.c_str());
            if (handle) return handle;

            // final failed
            return nullptr;
        }
    }
}

#endif //_INC_SEETA_AIP_DLL_H
