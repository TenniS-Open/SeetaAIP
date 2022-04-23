# Seeta AIP

[![License](https://img.shields.io/badge/license-BSD-blue.svg)](LICENSE)

AIP(AI Package) is the base of SeetaAIoT framework.

It provide single AI ability, which can be compute on satisfied device.

All AIPs will compiled to an shared library with interface `seeta_aip_load`, which has been describe in [seeta_aip.h](include/seeta_aip.h).

Use `seeta_aip_load` to load shared library and enjoy the AI.

Some language may throw exception about "Can not load library kernel32" in non-Windows system.
Compile `module/kernel32.cpp` by cmake or other ways to get shared library `kernel32`.
