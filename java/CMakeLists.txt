# build java target
find_package(JNI OPTIONAL_COMPONENTS)
if (JNI_FOUND)
    include_directories(${JAVA_INCLUDE_PATH})
    include_directories(${JAVA_INCLUDE_PATH2})
    message(STATUS "[STATUS] Compiling JNI module.")

    find_package (Python COMPONENTS Interpreter)
    message(STATUS "[STATUS] Found python: ${Python_EXECUTABLE}")

    set(JNI_HEADER_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/include")
    set(JNI_CLASS_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/class")

    file(MAKE_DIRECTORY ${JNI_CLASS_OUTPUT})

    set(java_depends)
    set(jni_depends)

    FILE(GLOB_RECURSE JNI_JAVA_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.java)
    foreach (path ${JNI_JAVA_FILES})
        # message(STATUS ${path})
        execute_process(COMMAND
                "${Python_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../script/javaname.py" "--class" "${path}"
                OUTPUT_VARIABLE output_class
                )
        execute_process(COMMAND
                "${Python_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../script/javaname.py" "--file" "${path}"
                OUTPUT_VARIABLE output_file
                )
        execute_process(COMMAND
                "${Python_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../script/javaname.py" "--header" "${path}"
                OUTPUT_VARIABLE output_header
                )

        if ("${output_class}" STREQUAL "")
            # No output public class
            message(STATUS "[WARNING] No public class found in ${path}")
            add_custom_command(OUTPUT "${path}.private"
                    COMMAND javac -d "${JNI_CLASS_OUTPUT}" "${path}"
                    DEPENDS "${path}"
                    COMMENT "javac ${path}"
                    VERBATIM
                    )
            list(APPEND java_depends "${path}.private")
        else()
            # Had output public class
            set(target_output_file "${JNI_CLASS_OUTPUT}/${output_file}")

            add_custom_command(OUTPUT "${target_output_file}"
                    COMMAND javac -d "${JNI_CLASS_OUTPUT}" "${path}"
                    DEPENDS "${path}"
                    COMMENT "javac ${path}"
                    VERBATIM
                    )

            set_source_files_properties("${target_output_file}" PROPERTIES GENERATED TRUE)

            list(APPEND java_depends "${target_output_file}")

            set(target_output_header "${JNI_HEADER_OUTPUT}/${output_header}")

            add_custom_command(OUTPUT "${target_output_header}"
                    COMMAND javah -d "${JNI_HEADER_OUTPUT}" -classpath "${JNI_CLASS_OUTPUT}" "${output_class}"
                    DEPENDS "${target_output_file}"
                    COMMENT "javah ${output_class}"
                    VERBATIM)

            set_source_files_properties("${target_output_header}" PROPERTIES GENERATED TRUE)

            list(APPEND jni_depends "${target_output_header}")

        endif()

    endforeach ()
    add_custom_target(java
            DEPENDS ${java_depends})

    add_custom_target(jni
            DEPENDS ${jni_depends})

    add_dependencies(jni java)

    FILE(GLOB_RECURSE JNI_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
    add_library(seeta_aip_java SHARED ${JNI_SRC_FILES})

    target_include_directories(seeta_aip_java PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

    add_dependencies(seeta_aip_java jni)
else ()
    message(STATUS "[WARNING] Can not find JNI header, not going to compile JNI module.")
endif ()