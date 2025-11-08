# =============================================================================
# Project Setup
# =============================================================================
cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH THIS_PARENT_PATH)
get_property(PARENT_PROJECT_NAME DIRECTORY ${THIS_PARENT_PATH} PROPERTY PROJECT_NAME)

project(pingpong_game_utility)

# =============================================================================
# Source File Collection
# =============================================================================
file(GLOB SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.c")

file(GLOB HEADER_FILES "${THIS_PARENT_PATH}/include/${PROJECT_NAME}/*.hpp"
     "${THIS_PARENT_PATH}/include/${PROJECT_NAME}/*.h"
     )

file(GLOB RESOURCE_FILES LIST_DIRECTORIES false "${THIS_PARENT_PATH}/resources/*.*")

# =============================================================================
# Library Target Definition
# =============================================================================
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

# =============================================================================
# External Dependencies
# =============================================================================
find_path(RAPIDCSV_INCLUDE_DIRS "rapidcsv.h")
find_package(nlohmann_json CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(SFML COMPONENTS system window graphics audio CONFIG REQUIRED)
find_package(boost_thread CONFIG REQUIRED)
find_package(boost_asio CONFIG REQUIRED)
target_link_libraries(
    ${PROJECT_NAME} PUBLIC nlohmann_json::nlohmann_json magic_enum::magic_enum sfml-graphics
                           sfml-audio Boost::thread Boost::asio
    )

# =============================================================================
# Include Directories
# =============================================================================
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PARENT_PROJECT_NAME}/${PROJECT_NAME}>
           $<BUILD_INTERFACE:${THIS_PARENT_PATH}/include/${PROJECT_NAME}>
    INTERFACE $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/interface>
              $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/interface> ${RAPIDCSV_INCLUDE_DIRS}
    )

# =============================================================================
# Compiler Options and Warnings
# =============================================================================
target_set_warnings(
    TARGET ${PROJECT_NAME} ENABLE ${ENABLE_WARNINGS} AS_ERRORS ${ENABLE_WARNINGS_AS_ERRORS}
    )

# Position Independent Code is required for static libraries that might be
# linked into shared libraries
if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    target_compile_options(${PROJECT_NAME} PUBLIC -fPIC)
endif(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")

# =============================================================================
# Target Properties
# =============================================================================
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "PingPong Game")

# =============================================================================
# Code Quality Tools
# =============================================================================
if(ENABLE_CLANG_TIDY AND (THIS_COMPILER_GCC OR THIS_COMPILER_CLANG OR THIS_COMPILER_CLANG_CL))
    set_target_properties(
        ${PROJECT_NAME} PROPERTIES CXX_CLANG_TIDY
                                   "clang-tidy;--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy"
        )
endif()

# =============================================================================
# Source File Organization
# =============================================================================
target_sources(
    ${PROJECT_NAME}
    PRIVATE
        FILE_SET
        ${PROJECT_NAME}_headers
        TYPE
        HEADERS
        BASE_DIRS
        ${THIS_PARENT_PATH}/include
        FILES
        ${HEADER_FILES}
    )

target_sources(${PROJECT_NAME} PRIVATE ${RESOURCE_FILES})
source_group("Resource Files" FILES ${RESOURCE_FILES})

# =============================================================================
# Installation Rules
# =============================================================================
install(
    TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
    )

install(DIRECTORY ${THIS_PARENT_PATH}/resources DESTINATION "." COMPONENT Resources)
