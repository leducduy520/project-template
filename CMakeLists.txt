cmake_minimum_required(VERSION 3.26)

# set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
project(
    DuyApp
    VERSION 0.0.1
    LANGUAGES C CXX
    )

include(GNUInstallDirs)
include(FetchContent)
include(TestCXXAcceptsFlag)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MESSAGE_LOG_LEVEL VERBOSE)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

foreach(build_type IN LISTS CMAKE_CONFIGURATION_TYPES)
    string(TOUPPER ${build_type} build_type_upper)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${build_type_upper} ${CMAKE_BINARY_DIR}/${build_type}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${build_type_upper} ${CMAKE_BINARY_DIR}/${build_type}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${build_type_upper} ${CMAKE_BINARY_DIR}/${build_type}/bin)
endforeach()

option(ENABLE_CMAKE_FORMAT "enable cmake format" ON)
option(ENABLE_DOCUMENT_TARGET "enable document target" ON)

set(CMAKE_EXECUTE_PROCESS_COMMAND_ECHO STDERR)
set(GRAPH_OUTPUT_NAME graph)

include(Tools)
add_cmake_format_target()

include(CustomTarget)
add_gen_doc()

add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(app)

include(CTest)
enable_testing()
add_subdirectory(test)

install(
    FILES ${DOCS_DIR}/graph/graph.png
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
    PERMISSIONS OWNER_WRITE WORLD_READ WORLD_EXECUTE
    )

install(
    DIRECTORY ${DOCS_DIR}/html
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
    DIRECTORY_PERMISSIONS OWNER_WRITE WORLD_READ WORLD_EXECUTE
    )
