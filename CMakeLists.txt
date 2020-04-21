cmake_minimum_required(VERSION 3.14)
project(seeta_aip)

set(CMAKE_CXX_STANDARD 11)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# compile shared libraries
add_library(test_aip_shared SHARED
        test/seeta_aip_package.cpp
        )

# compile test
add_executable(seeta_aip
        test/main.cpp
        )