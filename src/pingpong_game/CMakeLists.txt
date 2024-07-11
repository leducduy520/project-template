project(pingpong_game)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE_FILES)
list(APPEND GAME_SOURCE_FILE ${SOURCE_FILES})
list(APPEND UTIL_SOURCE_FILES ${SOURCE_FILES})
list(FILTER GAME_SOURCE_FILE INCLUDE REGEX "Game")
list(FILTER UTIL_SOURCE_FILES EXCLUDE REGEX "Game")
unset(SOURCE_FILES)

file(GLOB_RECURSE HEADER_FILES LIST_DIRECTORIES false "*.hpp")
list(APPEND GAME_HEADER_FILE ${HEADER_FILES})
list(APPEND UTIL_HEADER_FILES ${HEADER_FILES})
list(FILTER GAME_HEADER_FILE INCLUDE REGEX "Game")
list(FILTER UTIL_HEADER_FILES EXCLUDE REGEX "Game")
unset(HEADER_FILES)

file(GLOB RESOURCE_FILES LIST_DIRECTORIES false
     "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.*"
     )

add_library(${PROJECT_NAME}_util STATIC ${UTIL_SOURCE_FILES})
target_link_libraries(
    ${PROJECT_NAME}_util sfml-graphics sfml-audio rapidcsv magic_enum
    )
target_include_directories(
    ${PROJECT_NAME}_util
    PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    INTERFACE $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/interface>
              $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/interface>
    )
target_sources(
    ${PROJECT_NAME}_util PRIVATE FILE_SET ${PROJECT_NAME}_util_headers TYPE
                                 HEADERS FILES "${UTIL_HEADER_FILES}"
    )
target_sources(${PROJECT_NAME}_util PRIVATE ${RESOURCE_FILES})
source_group("Resource Files" FILES ${RESOURCE_FILES})
target_set_warnings(
    TARGET ${PROJECT_NAME}_util ENABLE ${ENABLE_WARNINGS} AS_ERRORS
    ${ENABLE_WARNINGS_AS_ERRORS}
    )
if(WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME}_util
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy
            ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        VERBATIM
        )
    add_custom_command(
        TARGET ${PROJECT_NAME}_util
        POST_BUILD
        COMMENT "Copy resources"
        COMMAND
            ${CMAKE_COMMAND} -E copy_directory_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/resources
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/../resources
        VERBATIM
        )
endif()

add_library(${PROJECT_NAME} SHARED ${GAME_SOURCE_FILE})
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_util)
target_sources(
    ${PROJECT_NAME} PRIVATE FILE_SET ${PROJECT_NAME}_headers TYPE HEADERS FILES
                            "${GAME_HEADER_FILE}"
    )
set_target_properties(
    ${PROJECT_NAME} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON
                               PUBLIC_HEADER "${GAME_HEADER_FILE}"
    )
target_set_warnings(
    TARGET ${PROJECT_NAME}_util ENABLE ${ENABLE_WARNINGS} AS_ERRORS
    ${ENABLE_WARNINGS_AS_ERRORS}
    )

if(CLANG_TIDY_CHECK)
    set_target_properties(
        ${PROJECT_NAME}_util
        PROPERTIES CXX_CLANG_TIDY
                   "clang-tidy;--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy"
        )

    set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES CXX_CLANG_TIDY
                   "clang-tidy;--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy"
        )
endif(CLANG_TIDY_CHECK)

install(
    TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_util
    EXPORT ${PROJECT_NAME}-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
                  COMPONENT PublicHeader
    )

install(DIRECTORY resources/ DESTINATION "resources" COMPONENT Resources)
