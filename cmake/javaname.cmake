# now find or compile javaname exe on JAVA_NAME_COMMAND

set(JAVA_NAME_SOURCE "${PROJECT_SOURCE_DIR}/tools/javaname.cpp")

if (WIN32)
    set(JAVA_NAME_OUTPUT "${PROJECT_SOURCE_DIR}/bin/javaname.exe")
else()
    set(JAVA_NAME_OUTPUT "${PROJECT_SOURCE_DIR}/bin/javaname")
endif ()

set(JAVA_NAME_OUTPUT_ROOT "${PROJECT_SOURCE_DIR}/bin")

# outer compare timestamp
file(TIMESTAMP "${JAVA_NAME_COMMAND}" JAVA_NAME_EXECUTE_TIME)
file(TIMESTAMP "${JAVA_NAME_SOURCE}" JAVA_NAME_SOURCE_TIME)
string(COMPARE GREATER_EQUAL "${JAVA_NAME_SOURCE_TIME}" "${JAVA_NAME_EXECUTE_TIME}" JAVA_NAME_OUT_OF_DATE)

# built JAVA_NAME_COMMAND
if ("${JAVA_NAME_COMMAND}" STREQUAL "" OR
        "${JAVA_NAME_COMMAND}" STREQUAL "JAVA_NAME_COMMAND-NOTFOUND" OR
        ${JAVA_NAME_OUT_OF_DATE})

    if (${JAVA_NAME_OUT_OF_DATE})
    else ()
        message(STATUS "Detecting javaname")
    endif ()

    find_program(JAVA_NAME_COMMAND javaname "${JAVA_NAME_OUTPUT_ROOT}")

    # compare time again
    file(TIMESTAMP "${JAVA_NAME_COMMAND}" JAVA_NAME_EXECUTE_TIME)
    file(TIMESTAMP "${JAVA_NAME_SOURCE}" JAVA_NAME_SOURCE_TIME)
    string(COMPARE GREATER_EQUAL "${JAVA_NAME_SOURCE_TIME}" "${JAVA_NAME_EXECUTE_TIME}" JAVA_NAME_OUT_OF_DATE)

    if ("${JAVA_NAME_COMMAND}" STREQUAL "JAVA_NAME_COMMAND-NOTFOUND" OR
            ${JAVA_NAME_OUT_OF_DATE})
        message(STATUS "Generating javaname")
        # compile javaname
        if (MSVC)
            execute_process(
                    COMMAND ${CMAKE_CXX_COMPILER} "${JAVA_NAME_SOURCE}"
                    /link/OUT:"${JAVA_NAME_OUTPUT}")
        else ()
            execute_process(
                    COMMAND ${CMAKE_CXX_COMPILER} "${JAVA_NAME_SOURCE}"
                    -o "${JAVA_NAME_OUTPUT}")
        endif()

        find_program(JAVA_NAME_COMMAND javaname "${JAVA_NAME_OUTPUT_ROOT}")
        if ("${JAVA_NAME_COMMAND}" STREQUAL "JAVA_NAME_COMMAND-NOTFOUND")
            message(FATAL "Can not found or generate execute javaname!")
        endif()
    endif()
endif()
