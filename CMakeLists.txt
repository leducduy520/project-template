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

foreach(build_type in ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${build_type} build_type_upper)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${build_type_upper} ${CMAKE_BINARY_DIR}/${build_type}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${build_type_upper} ${CMAKE_BINARY_DIR}/${build_type}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${build_type_upper} ${CMAKE_BINARY_DIR}/${build_type}/bin)
endforeach()

# CHECK_CXX_ACCEPTS_FLAG(<flags> <variable>)
# check_cxx_accepts_flag(/W4 AVALIABLE)

# if(AVALIABLE)
#     list(APPEND CMAKE_MESSAGE_INDENT "NOTICE:")
#     message(STATUS "/W4 is avaliable")
#     list(POP_BACK CMAKE_MESSAGE_INDENT)
#     set(CMAKE_CXX_FLAGS "/W4 /wd4530 /we4244")
# endif()

option(ENABLE_CMAKE_FORMAT "enable cmake format" ON)
option(ENABLE_DOCUMENT_TARGET "enable document target" ON)

set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/external)
set(DOCS_DIR ${CMAKE_SOURCE_DIR}/docs)
set(TOOLS_DIR ${CMAKE_SOURCE_DIR}/tools)

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
