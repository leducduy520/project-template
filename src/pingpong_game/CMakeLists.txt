# Project setup
cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH THIS_PARENT_PATH)

project(pingponggame)

# Dependencies
add_subdirectory(utility)
add_subdirectory(resources)

# Source files
set(SOURCE_FILES
    src/LoginGame.cpp
    src/PingPongGame.cpp
)

set(HEADER_FILES
    include/LoginGame.hpp
    include/PingPongGame.hpp
)

# Target definition
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC pingponggameutility ipc_lib)

add_dependencies(${PROJECT_NAME} generate_manifest)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/utility/include>
        $<BUILD_INTERFACE:${THIS_PARENT_PATH}/ipc/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/utility>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/ipc>
)

target_set_warnings(
    TARGET ${PROJECT_NAME}
    ENABLE ${ENABLE_WARNINGS}
    AS_ERRORS ${ENABLE_WARNINGS_AS_ERRORS}
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        PUBLIC_HEADER "${CMAKE_CURRENT_LIST_DIR}/include/PingPongGame.hpp"
        PREFIX ""
        FOLDER "PingPong Game"
        VS_DEBUGGER_ENVIRONMENT "AUTHOR=DuyLD"
)

if(ENABLE_CLANG_TIDY AND (THIS_COMPILER_GCC OR THIS_COMPILER_CLANG OR THIS_COMPILER_CLANG_CL))
    set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
            CXX_CLANG_TIDY "clang-tidy;--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy"
    )
endif()

target_sources(
    ${PROJECT_NAME}
    PRIVATE
        FILE_SET ${PROJECT_NAME}_headers
        TYPE HEADERS
        FILES ${HEADER_FILES}
)

# Installation
install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME} COMPONENT Development
)

# Miscellaneous
set_directory_properties(PROPERTIES VS_STARTUP_PROJECT ${PROJECT_NAME})