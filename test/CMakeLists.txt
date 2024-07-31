project(Catch-test)

FetchContent_Declare(
    Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git GIT_TAG v3.6.0
)
FetchContent_MakeAvailable(Catch2)

add_executable(${PROJECT_NAME} test.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE pingpong_game)

target_link_libraries(${PROJECT_NAME} PRIVATE Catch2WithMain)

set_target_properties(
    ${PROJECT_NAME} PROPERTIES INSTALL_RPATH
    ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
)

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(${PROJECT_NAME})

file(GLOB RESOURCE_FILES LIST_DIRECTORIES false
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.*"
)
target_sources(${PROJECT_NAME} PUBLIC "${RESOURCE_FILES}")
source_group("Resource Files" FILES ${RESOURCE_FILES})

if(WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMENT "Copy resources"
        COMMAND
        ${CMAKE_COMMAND} -E copy_directory_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/../resources
        VERBATIM
    )
endif()

install(
    TARGETS ${PROJECT_NAME} Catch2 Catch2WithMain
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
)

install(DIRECTORY resources DESTINATION ".")
