cmake_minimum_required(VERSION 3.14)
project(seeta_aip)

set(CMAKE_CXX_STANDARD 11)

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
endif()

if (WIN32)
elseif (IOS)
elseif (APPLE)
elseif (UNIX)
    list(APPEND third dl)
else ()
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