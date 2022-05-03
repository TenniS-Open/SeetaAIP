cmake_minimum_required(VERSION 3.1)
project(seeta_aip)

set(CMAKE_CXX_STANDARD 11)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(third)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR
        "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "NVIDIA")
endif()

if (WIN32)
elseif (ANDROID)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-command-line-argument")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-command-line-argument")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -frtti")
elseif (IOS)
elseif (APPLE)
elseif (UNIX)
    list(APPEND third dl)
else ()
endif ()

find_package(OpenMP)
if (OpenMP_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    include_directories(${OpenMP_CXX_INCLUDE_DIRS})
    link_libraries(${OpenMP_CXX_LIBRARIES})
endif()

add_subdirectory(java)

find_package(OpenCV OPTIONAL_COMPONENTS QUIET)

if (OpenCV_FOUND )
    include_directories(${OpenCV_INCLUDES})
    list(APPEND third ${OpenCV_LIBRARIES})
    message(STATUS "[STATUS] Compiling with OpenCV.")
else ()
    message(STATUS "[WARNING] Can not find OpenCV libraries, some test project would failed.")
endif ()

FILE(GLOB_RECURSE MODULE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/module/*.cpp)
foreach (path ${MODULE_FILES})
    string(REGEX MATCH "[^/]*.[(c)|(cc)|(cpp)]$" file_ext ${path})
    string(REGEX MATCH "^[^.]*" file ${file_ext})
    add_library(${file} SHARED ${path})
    target_compile_definitions(${file} PRIVATE SEETA_AIP_EXPORTS)
    if (WIN32)
        set_target_properties(${file} PROPERTIES PREFIX "")
    endif ()
endforeach ()

FILE(GLOB_RECURSE TEST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp)
foreach (path ${TEST_FILES})
    string(REGEX MATCH "[^/]*.[(c)|(cc)|(cpp)]$" file_ext ${path})
    string(REGEX MATCH "^[^.]*" file ${file_ext})
    add_executable(test_${file} ${path})
    target_link_libraries(test_${file} ${third})
endforeach ()

