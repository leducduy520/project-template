project(pingpong_game)

include(${CMAKE_CURRENT_LIST_DIR}/FetchLib.cmake)
fetchlib()

set(EXTERNAL_PROJECT_DIR)

if(NOT DEFINED ENV{EXTERNAL_PROJECT_DIR})
    set(EXTERNAL_PROJECT_DIR "${CMAKE_SOURCE_DIR}/external")
else()
    set(EXTERNAL_PROJECT_DIR "$ENV{EXTERNAL_PROJECT_DIR}")
endif()

message(STATUS "EXTERNAL_PROJECT_DIR: ${EXTERNAL_PROJECT_DIR}")

list(APPEND CMAKE_PREFIX_PATH "${EXTERNAL_PROJECT_DIR}/mongo-cxx-driver" "${EXTERNAL_PROJECT_DIR}/SFML")
find_package(mongocxx REQUIRED)
find_package(SFML REQUIRED COMPONENTS graphics audio)

add_subdirectory(utility)

file(GLOB SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.c")
file(GLOB HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/include/*.h"
)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC pingpong_game_utility)
target_include_directories(
    ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
)

target_set_warnings(
    TARGET ${PROJECT_NAME} ENABLE ${ENABLE_WARNINGS} AS_ERRORS ${ENABLE_WARNINGS_AS_ERRORS}
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS ON
    PUBLIC_HEADER "${CMAKE_CURRENT_LIST_DIR}/include/PingPongGame.hpp"
    PREFIX ""
    FOLDER "PingPong Game"
    VS_DEBUGGER_ENVIRONMENT "AUTHOR=DuyLD"
)

if(ENABLE_CLANG_TIDY AND(THIS_COMPILER_GCC OR THIS_COMPILER_CLANG OR THIS_COMPILER_CLANG_CL))
    set_target_properties(
        ${PROJECT_NAME} PROPERTIES CXX_CLANG_TIDY
        "clang-tidy;--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy"
    )
endif()

target_sources(
    ${PROJECT_NAME} PRIVATE FILE_SET ${PROJECT_NAME}_headers TYPE HEADERS FILES "${HEADER_FILES}"
)

if(THIS_OS_WINDOWS)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        COMMENT " Copy OpenAL DLL "
        PRE_BUILD
        COMMAND
        ${CMAKE_COMMAND} -E copy
        "${EXTERNAL_PROJECT_DIR}/SFML/bin/openal32.dll" $<TARGET_FILE_DIR:${PROJECT_NAME}>
        VERBATIM
    )

    install(FILES "${EXTERNAL_PROJECT_DIR}/SFML/bin/openal32.dll" DESTINATION ${CMAKE_INSTALL_BINDIR})

    # add_custom_command(
    # TARGET ${PROJECT_NAME}
    # POST_BUILD
    # COMMENT "Copy mongocxx driver"
    # COMMAND
    # ${CMAKE_COMMAND} -E copy_directory_if_different
    # "${CMAKE_SOURCE_DIR}/external/mongo-cxx-driver/bin" $<TARGET_FILE_DIR:${PROJECT_NAME}>
    # COMMAND
    # VERBATIM
    # )
endif()

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME} COMPONENT PublicHeader
)
