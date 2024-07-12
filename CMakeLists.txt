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

cmake_dependent_option(INSTALL_DOCS "install docs" OFF "NOT BUILD_DOCS" OFF)
cmake_dependent_option(INSTALL_GRAPH "install graph" OFF "NOT BUILD_GRAPH" OFF)
cmake_dependent_option(
    CLANG_TIDY_CHECK "clang_tidy_check" ON
    "ENABLE_CLANG_TIDY AND NOT ENABLE_WARNINGS" OFF
    )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MESSAGE_LOG_LEVEL VERBOSE)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_EXECUTE_PROCESS_COMMAND_ECHO STDOUT)

set(INSTALL_CONFIG_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

if(UNIX)
    set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
endif(UNIX)

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_COLOR_DIAGNOSTICS ON)
    message("CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}")
endif(NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")

foreach(build_type IN LISTS CMAKE_CONFIGURATION_TYPES)
    string(TOUPPER ${build_type} build_type_upper)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${build_type_upper}
        ${CMAKE_BINARY_DIR}/${build_type}/lib
        )
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${build_type_upper}
        ${CMAKE_BINARY_DIR}/${build_type}/lib
        )
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${build_type_upper}
        ${CMAKE_BINARY_DIR}/${build_type}/bin
        )
endforeach()

include(Tools)
include(Document)
include(Warnings)

FetchContent_Declare(
    SFML GIT_REPOSITORY https://github.com/SFML/SFML.git GIT_TAG 2.6.x
    )
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(
    rapidcsv GIT_REPOSITORY https://github.com/d99kris/rapidcsv.git
    GIT_TAG v8.82
    )
FetchContent_MakeAvailable(rapidcsv)

# FetchContent_Declare(
# SQLiteCpp GIT_REPOSITORY https://github.com/SRombauts/SQLiteCpp.git
# GIT_TAG 3.3.1
# )
# FetchContent_MakeAvailable(SQLiteCpp)
FetchContent_Declare(
    magic_enum GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG v0.9.6
    )
FetchContent_MakeAvailable(magic_enum)

add_subdirectory(src)
add_subdirectory(app)

if(BUILD_TEST)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
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
