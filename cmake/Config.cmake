if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(THIS_OS_WINDOWS 1)
    message(STATUS "WINDOWS OS")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(THIS_OS_LINUX 1)
    message(STATUS "LINUX OS")
endif()

if(MSVC)
    set(THIS_COMPILER_MSVC 1)

    if(MSVC_VERSION EQUAL 1400)
        set(THIS_MSVC_VERSION 8)
    elseif(MSVC_VERSION EQUAL 1500)
        set(THIS_MSVC_VERSION 9)
    elseif(MSVC_VERSION EQUAL 1600)
        set(THIS_MSVC_VERSION 10)
    elseif(MSVC_VERSION EQUAL 1700)
        set(THIS_MSVC_VERSION 11)
    elseif(MSVC_VERSION EQUAL 1800)
        set(THIS_MSVC_VERSION 12)
    elseif(MSVC_VERSION EQUAL 1900)
        set(THIS_MSVC_VERSION 14)
    elseif(MSVC_VERSION LESS_EQUAL 1919)
        set(THIS_MSVC_VERSION 15)
    elseif(MSVC_VERSION LESS_EQUAL 1929)
        set(THIS_MSVC_VERSION 16)
    elseif(MSVC_VERSION LESS_EQUAL 1939)
        set(THIS_MSVC_VERSION 17)
    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(THIS_COMPILER_CLANG_CL 1)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(THIS_COMPILER_CLANG 1)

    execute_process(
        COMMAND "${CMAKE_CXX_COMPILER}" "--version" OUTPUT_VARIABLE CLANG_VERSION_OUTPUT
        )
    string(
        REGEX REPLACE ".*clang version ([0-9]+\\.[0-9]+).*" "\\1" THIS_CLANG_VERSION
                      "${CLANG_VERSION_OUTPUT}"
        )

    execute_process(
        COMMAND "${CMAKE_CXX_COMPILER}" "-v" OUTPUT_VARIABLE CLANG_COMPILER_VERSION
        ERROR_VARIABLE CLANG_COMPILER_VERSION
        )
elseif(CMAKE_COMPILER_IS_GNUCXX)
    set(THIS_COMPILER_GCC 1)

    execute_process(
        COMMAND "${CMAKE_CXX_COMPILER}" "-dumpversion" OUTPUT_VARIABLE GCC_VERSION_OUTPUT
        )
    string(REGEX REPLACE "([0-9]+\\.[0-9]+).*" "\\1" THIS_GCC_VERSION "${GCC_VERSION_OUTPUT}")

    execute_process(
        COMMAND "${CMAKE_CXX_COMPILER}" "-v" OUTPUT_VARIABLE GCC_COMPILER_VERSION
        ERROR_VARIABLE GCC_COMPILER_VERSION
        )
    string(REGEX MATCHALL ".*(tdm[64]*-[1-9]).*" THIS_COMPILER_GCC_TDM "${GCC_COMPILER_VERSION}")

    if("${GCC_COMPILER_VERSION}" MATCHES "ucrt")
        set(THIS_RUNTIME_UCRT 1)
    endif()

    execute_process(COMMAND "${CMAKE_CXX_COMPILER}" "-dumpmachine" OUTPUT_VARIABLE GCC_MACHINE)
    string(STRIP "${GCC_MACHINE}" GCC_MACHINE)

    if(GCC_MACHINE MATCHES ".*w64.*")
        set(THIS_COMPILER_GCC_W64 1)
    endif()
endif()
