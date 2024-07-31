project(pingpong_game)

include(${CMAKE_CURRENT_LIST_DIR}/FetchLib.cmake)
FetchLib()

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/external/msvc-cl/mongo-cxx-driver")
find_package(mongocxx REQUIRED)

# include("E:/VSC/a1_C_plus_plus/project-template/external/msvc-cl/mongo-cxx-driver/lib/cmake/mongocxx-3.10.2/mongocxx-config.cmake")
add_subdirectory(utility)

file(GLOB SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.c")
file(GLOB HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/include/*.hpp" "${CMAKE_CURRENT_LIST_DIR}/include/*.h")

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC pingpong_game_utility)
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
)

target_set_warnings(
    TARGET ${PROJECT_NAME} ENABLE ${ENABLE_WARNINGS} AS_ERRORS
    ${ENABLE_WARNINGS_AS_ERRORS}
)

set_target_properties(
    ${PROJECT_NAME} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON
    PUBLIC_HEADER "${CMAKE_CURRENT_LIST_DIR}/include/PingPongGame.hpp"
    PREFIX ""
    FOLDER "PingPong Game"
)

target_sources(
    ${PROJECT_NAME} PRIVATE FILE_SET ${PROJECT_NAME}_headers TYPE HEADERS FILES
    "${HEADER_FILES}"
)

if(WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        COMMENT " Copy OpenAL DLL "
        PRE_BUILD
        COMMAND
        ${CMAKE_COMMAND} -E copy
        ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        VERBATIM
    )

    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMENT "Copy mongocxx driver"
        COMMAND
        ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_SOURCE_DIR}/external/msvc-cl/mongo-cxx-driver/bin"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND
        VERBATIM
    )
endif()

if(ENABLE_CLANG_TIDY)
    set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES CXX_CLANG_TIDY
        "clang-tidy;--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy"
    )
endif(ENABLE_CLANG_TIDY)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
    COMPONENT PublicHeader
)
