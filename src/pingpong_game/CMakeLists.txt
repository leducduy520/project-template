project(pingpong_game)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE_FILES)
file(GLOB_RECURSE PUBLIC_HEADER_FILE LIST_DIRECTORIES false "*.hpp")
list(APPEND HEADER_FILES "${PUBLIC_HEADER_FILE}")
list(FILTER PUBLIC_HEADER_FILE INCLUDE REGEX "Game")
file(GLOB RESOURCE_FILES LIST_DIRECTORIES false "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.*")

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../interface>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/interface>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>)

target_sources(${PROJECT_NAME} PRIVATE
    FILE_SET ${PROJECT_NAME}_headers
    TYPE HEADERS
    FILES "${HEADER_FILES}"
)

target_sources(${PROJECT_NAME} PRIVATE
    ${RESOURCE_FILES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES 
    WINDOWS_EXPORT_ALL_SYMBOLS ON
    PUBLIC_HEADER "${PUBLIC_HEADER_FILE}"
    )

source_group("Resource Files" FILES ${RESOURCE_FILES})

target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-audio rapidcsv SQLiteCpp magic_enum)

if(WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
        VERBATIM
        )
    add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMENT "Copy resources"
            COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_CURRENT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/../resources
            VERBATIM
        )
endif()

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
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
        COMPONENT PublicHeader
)

install(DIRECTORY resources/ DESTINATION "resources" COMPONENT Resources)