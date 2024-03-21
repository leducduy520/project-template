cmake_minimum_required(VERSION 3.27)

project(external)

include(ExternalProject)

set_property(DIRECTORY PROPERTY EP_PREFIX ${CMAKE_SOURCE_DIR})

if(WIN32)
    set(PRESET windows)
else()
    set(PRESET ubuntu)
endif()

set(PKG_NAME catch2)

ExternalProject_Add(
    catch2
    PREFIX ${CMAKE_SOURCE_DIR}
    SOURCE_DIR  ${CMAKE_SOURCE_DIR}/src/${PKG_NAME}
    BINARY_DIR  ${CMAKE_SOURCE_DIR}/src/${PKG_NAME}-build-${PRESET}
    STAMP_DIR   ${CMAKE_SOURCE_DIR}/src/${PKG_NAME}-stamp-${PRESET}
    TMP_DIR     ${CMAKE_SOURCE_DIR}/tmp
    GIT_REPOSITORY https://github.com/leducduy520/Catch2
    GIT_TAG dev_dld
    GIT_SHALLOW YES
    GIT_PROGRESS YES
    CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=install"
    USES_TERMINAL_DOWNLOAD YES
    USES_TERMINAL_UPDATE YES
    USES_TERMINAL_CONFIGURE YES
    USES_TERMINAL_BUILD YES
    USES_TERMINAL_INSTALL YES
)

ExternalProject_Get_Property(catch2 SOURCE_DIR)
set(CATCH2_SOURCE_DIR
${SOURCE_DIR}
CACHE PATH "CATCH2_SOURCE_DIR"
)

ExternalProject_Get_Property(catch2 BINARY_DIR)
set(CATCH2_BINARY_DIR
    ${BINARY_DIR}
    CACHE PATH "CATCH2_BINARY_DIR"
)

# set(CATCH2_INSTALL_PREFIX
# ${CATCH2_BINARY_DIR}/install
# CACHE PATH "CATCH2_INSTALL_PREFIX"
# )
