cmake_minimum_required(VERSION 3.25)

# set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
option(ENABLE_CMAKE_FORMAT "enable cmake format" ON)
option( "enable document target" OFF)
option(ENABLE_IENABLE_DOCUMENT_TARGETNSTALL_GRAPH_AND_DOC "enable moving files in docs directory to the install directory" OFF)
option(ENABLE_TEST_EXEC "enable build test executable" OFF)

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

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MESSAGE_LOG_LEVEL VERBOSE)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_EXECUTE_PROCESS_COMMAND_ECHO STDOUT)

set(INSTALL_CONFIG_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
set(GRAPH_OUTPUT_NAME graph)

if(UNIX)
    set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
endif(UNIX)

foreach(build_type IN LISTS CMAKE_CONFIGURATION_TYPES)
    string(TOUPPER ${build_type} build_type_upper)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${build_type_upper} ${CMAKE_BINARY_DIR}/${build_type}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${build_type_upper} ${CMAKE_BINARY_DIR}/${build_type}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${build_type_upper} ${CMAKE_BINARY_DIR}/${build_type}/bin)
endforeach()

# include(Tools)
# include(GraphAndDoc)
# include(External)

FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(rapidcsv
    GIT_REPOSITORY https://github.com/d99kris/rapidcsv.git
    GIT_TAG v8.82 )
FetchContent_MakeAvailable(rapidcsv)

add_subdirectory(src)
add_subdirectory(app)

if(ENABLE_TEST_EXEC)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif(ENABLE_TEST_EXEC)

# if(NOT SUBPROJECT)
#     configure_package_config_file(
#         ${CMAKE_CURRENT_LIST_DIR}/cmake/ZooClassConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/ZooClassConfig.cmake
#         INSTALL_DESTINATION ${INSTALL_CONFIG_DIR}
#         )

#     write_basic_package_version_file(
#         "${CMAKE_CURRENT_BINARY_DIR}/ZooClassConfigVersion.cmake" COMPATIBILITY SameMajorVersion
#         )

#     install(
#         FILES ${CMAKE_CURRENT_BINARY_DIR}/ZooClassConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/ZooClassConfigVersion.cmake
#         DESTINATION ${INSTALL_CONFIG_DIR}
#         )
# endif(NOT SUBPROJECT)

set(MY_PROJECT_VERSION_MAJOR 1)
set(MY_PROJECT_VERSION_MINOR 0)
set(MY_PROJECT_VERSION_PATCH 0)
set(MY_PROJECT_VERSION ${MY_PROJECT_VERSION_MAJOR}.${MY_PROJECT_VERSION_MINOR}.${MY_PROJECT_VERSION_PATCH})

# CPack configuration
include(CPack)

# Set the name and version of the package
set(CPACK_PACKAGE_NAME "DuyLDGame")
set(CPACK_PACKAGE_VERSION ${MY_PROJECT_VERSION})
set(CPACK_PACKAGE_CONTACT "duyleduc123@gmail.com")

# Set the package file name without a specific generator suffix
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")

# Set the package description summary
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MyProject is an example project to demonstrate CPack usage.")