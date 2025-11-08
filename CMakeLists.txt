cmake_minimum_required(VERSION 3.25)

if(PROJECT_NAME)
    set(SUBPROJECT ON)
else(PROJECT_NAME)
    set(SUBPROJECT OFF)
endif()

project(GraphicApp VERSION 0.0.1 LANGUAGES C CXX)

include(GNUInstallDirs)
include(FetchContent)
include(TestCXXAcceptsFlag)
include(CMakePackageConfigHelpers)
include(CMakeDependentOption)

# set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
option(BUILD_CMAKE_FORMAT "enable cmake format" ON)
option(BUILD_GRAPH "build graph" ON)
option(BUILD_DOCS "build documentation" ON)
option(BUILD_TEST "enable build test executable" ON)
option(BUILD_CLANG_FORMAT "enable clang format" ON)
option(ENABLE_WARNINGS "enable warnings" ON)
option(ENABLE_WARNINGS_AS_ERRORS "enable warnings as errors" OFF)
option(ENABLE_CLANG_TIDY "enable clang tidy" OFF)
option(ENABLE_SANITIZE_ADDR "enable address sanitizer" OFF)
option(ENABLE_SANITIZE_UNDEF "enable undefined behavior sanitizer" OFF)

cmake_dependent_option(INSTALL_DOCS "install docs" OFF "NOT BUILD_DOCS" OFF)
cmake_dependent_option(INSTALL_GRAPH "install graph" OFF "NOT BUILD_GRAPH" OFF)

# Set CXX standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set CMake variables
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MESSAGE_LOG_LEVEL VERBOSE)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(X_VCPKG_APPLOCAL_DEPS_INSTALL CACHE BOOL "Enable app local deps install" ON)

# Set custom variables
set(INSTALL_CONFIG_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

if(NOT DEFINED ENV{EXTERNAL_PROJECT_DIR} AND NOT DEFINED EXTERNAL_PROJECT_DIR)
    set(EXTERNAL_PROJECT_DIR "${CMAKE_SOURCE_DIR}/external"
        CACHE PATH "Where to file external projects" FORCE
        )
elseif(DEFINED ENV{EXTERNAL_PROJECT_DIR} AND NOT DEFINED EXTERNAL_PROJECT_DIR)
    set(EXTERNAL_PROJECT_DIR "$ENV{EXTERNAL_PROJECT_DIR}"
        CACHE PATH "Where to file external projects" FORCE
        )
endif()

message(STATUS "EXTERNAL_PROJECT_DIR: ${EXTERNAL_PROJECT_DIR}")

# Set output directory for each configuration
get_property(THIS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

# add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

include(Config)
include(Warnings)

include(Tools)
build_clang_format_target()
build_cmake_format_target()

include(Document)
add_project_documentation()

include(Sanitizer)
add_sanitizer_flags()

if(THIS_OS_LINUX)
    set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
endif(THIS_OS_LINUX)

add_subdirectory(src)
add_subdirectory(app)

if(BUILD_TEST)
    include(CTest)
    enable_testing()
    # add_subdirectory(test)
endif(BUILD_TEST)

# if(NOT SUBPROJECT)
# configure_package_config_file(
# ${CMAKE_CURRENT_LIST_DIR}/cmake/ZooClassConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/ZooClassConfig.cmake
# INSTALL_DESTINATION ${INSTALL_CONFIG_DIR}
# )

# write_basic_package_version_file(
# "${CMAKE_CURRENT_BINARY_DIR}/ZooClassConfigVersion.cmake" COMPATIBILITY SameMajorVersion
# )

# install(
# FILES ${CMAKE_CURRENT_BINARY_DIR}/ZooClassConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/ZooClassConfigVersion.cmake
# DESTINATION ${INSTALL_CONFIG_DIR}
# )
# endif(NOT SUBPROJECT)
