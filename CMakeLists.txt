cmake_minimum_required(VERSION 3.14)
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
elseif (IOS)
elseif (APPLE)
elseif (UNIX)
    list(APPEND third dl)
else ()
endif ()

# set java name target for debug tools, well for cmake usage
add_executable(javaname "${CMAKE_CURRENT_SOURCE_DIR}/tools/javaname.cpp")

# find jni for good
find_package(JNI OPTIONAL_COMPONENTS)
if (JNI_FOUND)
    include_directories(${JAVA_INCLUDE_PATH})
    include_directories(${JAVA_INCLUDE_PATH2})
    message(STATUS "[STATUS] Compiling JNI module.")

    include(javaname)

    FILE(GLOB_RECURSE JNI_JAVA_FILES ${CMAKE_CURRENT_SOURCE_DIR}/java/*.java)
    foreach (path ${JNI_JAVA_FILES})
        message(STATUS ${path})

        add_custom_command(OUTPUT "${path}.target"
                COMMAND javac "${path}" -d "${CMAKE_CURRENT_SOURCE_DIR}/java/"
                DEPENDS "${path}"
                COMMENT "Compiling ${path}"
                VERBATIM
                )
        add_custom_target(java
                DEPENDS "${path}.target")

    endforeach ()

    FILE(GLOB_RECURSE JNI_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/java/*.cpp)
    add_library(seeta_aip_java SHARED ${JNI_SRC_FILES})
else ()
    message(STATUS "[WARNING] Can not find JNI header, not going to compile JNI module.")
endif ()

find_package(OpenCV OPTIONAL_COMPONENTS)

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
endforeach ()

FILE(GLOB_RECURSE TEST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp)
foreach (path ${TEST_FILES})
    string(REGEX MATCH "[^/]*.[(c)|(cc)|(cpp)]$" file_ext ${path})
    string(REGEX MATCH "^[^.]*" file ${file_ext})
    add_executable(test_${file} ${path})
    target_link_libraries(test_${file} ${third})
endforeach ()