project(pingpong_game)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE_FILES)
file(GLOB_RECURSE HEADER_FILE LIST_DIRECTORIES false "*.hpp")
list(FILTER HEADER_FILE INCLUDE REGEX "Game")

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system)

set_target_properties(${PROJECT_NAME} PROPERTIES 
    WINDOWS_EXPORT_ALL_SYMBOLS ON
    PUBLIC_HEADER "${HEADER_FILE}")

if(WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
        VERBATIM)
endif()

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMENT "Copy resources"
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_CURRENT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
    VERBATIM
)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
    RUNTIME
        DESTINATION  ${CMAKE_INSTALL_BINDIR}
        COMPONENT Runtime
    ARCHIVE
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT Development
    LIBRARY
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT Development
    PUBLIC_HEADER
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/game2
        COMPONENT PublicHeader
)

install (DIRECTORY resources DESTINATION ${CMAKE_INSTALL_BINDIR})