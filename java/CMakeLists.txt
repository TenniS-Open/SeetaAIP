# build java target

# set java output package and library name
set(JAVA_PACKAGE_NAME seeta_aip)

# find jni for compile
find_package(JNI OPTIONAL_COMPONENTS)

if (JNI_FOUND)
    include_directories(${JAVA_INCLUDE_PATH})
    include_directories(${JAVA_INCLUDE_PATH2})
else ()
    if (ANDROID)
    else ()
        message(STATUS "[WARNING] Can not find JNI header, leave empty.")
    endif ()
endif ()

if (TRUE)
    message(STATUS "[STATUS] Compiling JNI module.")

    find_package(Java REQUIRED COMPONENTS Development)
    message(STATUS "Found Java ${Java_VERSION}")


    if (NOT Java_JAVA_EXECUTABLE)
        message(FATAL_ERROR "Can not found java.")
    endif ()
    if (NOT Java_JAVAC_EXECUTABLE)
        message(FATAL_ERROR "Can not found javac.")
    endif ()

    # check java version
    if ("${Java_VERSION_MAJOR}" VERSION_GREATER "8.")
        message(STATUS "Using javac -h instead javah")
    else ()
        if (NOT Java_JAVAH_EXECUTABLE)
            message(FATAL_ERROR "Can not found javah.")
        endif ()
    endif ()

    # found python to use script to help compile
    find_package (Python COMPONENTS Interpreter)
    message(STATUS "[STATUS] Found python: ${Python_EXECUTABLE}")

    set(Javac_options "-J-Duser.language=en")

    set(JNI_SRC_INPUT "${CMAKE_CURRENT_SOURCE_DIR}/src")
    set(JNI_TEST_INPUT "${CMAKE_CURRENT_SOURCE_DIR}/test")
    set(JNI_HEADER_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/include")
    set(JNI_CLASS_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/class")
    set(JNI_JAR_OUTPUT "${LIBRARY_OUTPUT_PATH}")
    set(JNI_TEST_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/test")

    file(MAKE_DIRECTORY ${JNI_CLASS_OUTPUT})

    set(java_depends)
    set(header_depends)
    set(pure_java_depends)

    FILE(GLOB_RECURSE JNI_JAVA_FILES ${JNI_SRC_INPUT}/*.java)
    foreach (input_java ${JNI_JAVA_FILES})
        # message(STATUS ${input_java})
        execute_process(COMMAND
                "${Python_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../script/javaname.py" "--class" "${input_java}"
                OUTPUT_VARIABLE class_signature
                )
        execute_process(COMMAND
                "${Python_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../script/javaname.py" "--file" "${input_java}"
                OUTPUT_VARIABLE output_class
                )
        execute_process(COMMAND
                "${Python_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../script/javaname.py" "--header" "${input_java}"
                OUTPUT_VARIABLE output_header
                )

        if ("${class_signature}" STREQUAL "")
            # No output public class
            message(STATUS "[WARNING] No public class found in ${input_java}")
            add_custom_command(OUTPUT "${input_java}.private"
                    COMMAND ${Java_JAVAC_EXECUTABLE}
                        -d "${JNI_CLASS_OUTPUT}"
                        -classpath "${JNI_CLASS_OUTPUT}"
                        -sourcepath "${JNI_SRC_INPUT}"
                        "${input_java}"
                        ${Javac_options}
                    DEPENDS "${input_java}"
                    COMMENT "javac ${input_java}"
                    VERBATIM
                    )
            set_source_files_properties("${input_java}.private" PROPERTIES GENERATED TRUE)
            list(APPEND java_depends "${input_java}.private")
        elseif ("${output_header}" STREQUAL "")
            # for java file has no native method
            set(target_output_class "${JNI_CLASS_OUTPUT}/${output_class}")
            add_custom_command(OUTPUT "${target_output_class}"
                    COMMAND ${Java_JAVAC_EXECUTABLE}
                    -d "${JNI_CLASS_OUTPUT}"
                    -classpath "${JNI_CLASS_OUTPUT}"
                    -sourcepath "${JNI_SRC_INPUT}"
                    "${input_java}"
                    ${Javac_options}
                    DEPENDS "${input_java}"
                    COMMENT "javac ${input_java}"
                    VERBATIM
                    )
            set_source_files_properties("${target_output_class}" PROPERTIES GENERATED TRUE)
            list(APPEND java_depends "${target_output_class}")
            list(APPEND pure_java_depends "${target_output_class}")
        else()
            # Had output public class
            set(target_output_class "${JNI_CLASS_OUTPUT}/${output_class}")
            set(target_output_header "${JNI_HEADER_OUTPUT}/${output_header}")

            if ("${Java_VERSION_MAJOR}" VERSION_GREATER "8.")
                add_custom_command(
                        OUTPUT
                            "${target_output_header}"
                            "${target_output_class}"
                        COMMAND ${Java_JAVAC_EXECUTABLE}
                        -d "${JNI_CLASS_OUTPUT}"
                        -h "${JNI_HEADER_OUTPUT}"
                        -classpath "${JNI_CLASS_OUTPUT}"
                        -sourcepath "${JNI_SRC_INPUT}"
                        "${input_java}"
                        ${Javac_options}
                        DEPENDS "${input_java}"
                        COMMENT "javac -h ${input_java}"
                        VERBATIM
                        )
            else ()
                add_custom_command(OUTPUT "${target_output_class}"
                        COMMAND ${Java_JAVAC_EXECUTABLE}
                            -d "${JNI_CLASS_OUTPUT}"
                            -classpath "${JNI_CLASS_OUTPUT}"
                            -sourcepath "${JNI_SRC_INPUT}"
                            "${input_java}"
                            ${Javac_options}
                        DEPENDS "${input_java}"
                        COMMENT "javac ${input_java}"
                        VERBATIM
                        )
                add_custom_command(OUTPUT "${target_output_header}"
                        COMMAND ${Java_JAVAH_EXECUTABLE}
                        -d "${JNI_HEADER_OUTPUT}"
                        -classpath "${JNI_CLASS_OUTPUT}"
                        "${class_signature}"
                        ${Javac_options}
                        DEPENDS "${target_output_class}"
                        COMMENT "javah ${class_signature}"
                        VERBATIM)
            endif ()

            set_source_files_properties("${target_output_class}" PROPERTIES GENERATED TRUE)
            set_source_files_properties("${target_output_header}" PROPERTIES GENERATED TRUE)

            list(APPEND java_depends "${target_output_class}")
            list(APPEND header_depends "${target_output_header}")
        endif()

    endforeach ()

    # add_custom_target(javac
    #         DEPENDS ${java_depends})

    add_custom_target(javah
            DEPENDS ${header_depends})

    # add_dependencies(javah javac)

    # set library target
    FILE(GLOB_RECURSE JNI_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
    add_library(${PROJECT_NAME}_java SHARED ${JNI_SRC_FILES})
    target_include_directories(${PROJECT_NAME}_java PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    add_dependencies(${PROJECT_NAME}_java javah)
    set_target_properties(${PROJECT_NAME}_java PROPERTIES OUTPUT_NAME ${JAVA_PACKAGE_NAME}_java)
    if (WIN32)
        set_target_properties(${PROJECT_NAME}_java PROPERTIES PREFIX "")
    endif ()

    set(target_output_jar "${JNI_JAR_OUTPUT}/${JAVA_PACKAGE_NAME}.jar")

    add_custom_command(OUTPUT "${target_output_jar}"
            COMMAND ${Java_JAR_EXECUTABLE}
                -cvf "${target_output_jar}"
                -C "${JNI_CLASS_OUTPUT}" .
                ${Javac_options}
            DEPENDS ${java_depends}
            COMMENT "jar ${target_output_jar}"
            VERBATIM)

    add_custom_target(jar
            DEPENDS "${target_output_jar}")

    # build test files
    FILE(GLOB_RECURSE JNI_TEST_FILES ${JNI_TEST_INPUT}/*.java)
    foreach (input_java ${JNI_TEST_FILES})
        # build test script
        execute_process(COMMAND
                "${Python_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../script/javaname.py" "--class" "${input_java}"
                OUTPUT_VARIABLE class_signature
                )
        execute_process(COMMAND
                "${Python_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../script/javaname.py" "--file" "${input_java}"
                OUTPUT_VARIABLE output_class
                )
        execute_process(COMMAND
                "${Python_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/../script/javaname.py" "--header" "${input_java}"
                OUTPUT_VARIABLE output_header
                )

        if ("${class_signature}" STREQUAL "")
            # No output public class
            message(STATUS "[WARNING] No public class found in ${input_java}")
        else()
            # Had output public class
            set(target_output_class "${JNI_TEST_OUTPUT}/${output_class}")

            add_custom_command(OUTPUT "${target_output_class}"
                    COMMAND ${Java_JAVAC_EXECUTABLE}
                        -classpath "${JNI_CLASS_OUTPUT}"
                        -sourcepath "${JNI_SRC_INPUT}"
                        -d "${JNI_TEST_OUTPUT}"
                        "${input_java}"
                        ${Javac_options}
                    DEPENDS javah
                        ${pure_java_depends}
                        "${input_java}"
                    COMMENT "javac ${input_java}"
                    VERBATIM)

            set_source_files_properties("${target_output_class}" PROPERTIES GENERATED TRUE)

            if (WIN32)
                set(TEST_CLASSPATH "${JNI_TEST_OUTPUT};${JNI_CLASS_OUTPUT}")
            else()
                set(TEST_CLASSPATH "${JNI_TEST_OUTPUT}:${JNI_CLASS_OUTPUT}")
            endif()

            add_custom_target("javac-${class_signature}"
                    DEPENDS "${target_output_class}")

            add_custom_target("java-${class_signature}"
                    COMMAND ${Java_JAVA_EXECUTABLE}
                        "-Djava.library.path=${LIBRARY_OUTPUT_PATH}"
                        -classpath "${TEST_CLASSPATH}"
                        "${class_signature}"
                        ${Javac_options}
                    DEPENDS "javac-${class_signature}"
                        ${PROJECT_NAME}_java
                    COMMENT "java ${class_signature}"
                    WORKING_DIRECTORY "${JNI_TEST_OUTPUT}"
                    VERBATIM)

            add_dependencies("java-${class_signature}" ${PROJECT_NAME}_java)

        endif()
    endforeach()
endif ()
