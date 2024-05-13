cmake_minimum_required(VERSION 3.26)

# set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
project(
    ZooClass
    VERSION 0.0.1
    LANGUAGES C CXX
    )

include(GNUInstallDirs)
include(FetchContent)
include(TestCXXAcceptsFlag)
include(CMakePackageConfigHelpers)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MESSAGE_LOG_LEVEL VERBOSE)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_EXECUTE_PROCESS_COMMAND_ECHO STDERR)

set(INSTALL_CONFIG_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
set(GRAPH_OUTPUT_NAME graph)
set(BASE_NAME base)

option(ENABLE_CMAKE_FORMAT "enable cmake format" ON)
option(ENABLE_DOCUMENT_TARGET "enable document target" ON)
option(ENABLE_MOVE_DOCS_TO_INSTALL "enable moving files in docs directory to the install directory" ON)

if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
else()
    set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_LIBDIR})
endif()

foreach(build_type IN LISTS CMAKE_CONFIGURATION_TYPES)
    string(TOUPPER ${build_type} build_type_upper)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${build_type_upper} ${CMAKE_BINARY_DIR}/${build_type}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${build_type_upper} ${CMAKE_BINARY_DIR}/${build_type}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${build_type_upper} ${CMAKE_BINARY_DIR}/${build_type}/bin)
endforeach()

include(Tools)
add_cmake_format_target()

include(CustomTarget)
add_gen_doc()

build_external_project(READ_FROM_JSON SUB_PROJECT)

add_subdirectory(src)

#add_subdirectory(test)

configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/cmake/ZooClassConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/ZooClassConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIG_DIR}
    )

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/ZooClassConfigVersion.cmake" COMPATIBILITY SameMajorVersion
    )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ZooClassConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/ZooClassConfigVersion.cmake
        DESTINATION ${INSTALL_CONFIG_DIR}
        )
