project(PingPong-test)

# Find Google Test from vcpkg
find_package(GTest CONFIG REQUIRED)

add_executable(${PROJECT_NAME} test.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE pingpong_game_utility)

# Link against Google Test
target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest GTest::gtest_main GTest::gmock)

set_target_properties(
    ${PROJECT_NAME} PROPERTIES INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
    )

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Test")

# Enable Google Test discovery for CTest
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})

file(GLOB RESOURCE_FILES LIST_DIRECTORIES false "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.*")
target_sources(${PROJECT_NAME} PUBLIC "${RESOURCE_FILES}")
source_group("Resource Files" FILES ${RESOURCE_FILES})

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMENT "Copy resources"
    COMMAND
        ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_CURRENT_SOURCE_DIR}/resources
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
    VERBATIM
    )

# Copy resources from pingpong_game/resources excluding *.txt files
set(PINGPONG_RESOURCES_DIR "${CMAKE_SOURCE_DIR}/src/pingpong_game/resources")

if(UNIX)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMENT "Copy pingpong_game resources (excluding *.txt)"
        COMMAND find ${PINGPONG_RESOURCES_DIR} -type f ! -name "*.txt" -exec ${CMAKE_COMMAND} -E copy {} $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources/ \;
        VERBATIM
    )
elseif(WIN32)
    # Windows version using PowerShell
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMENT "Copy pingpong_game resources (excluding *.txt)"
        COMMAND powershell -Command "Get-ChildItem -Path '${PINGPONG_RESOURCES_DIR}' -Recurse -File | Where-Object { $_.Extension -ne '.txt' } | Copy-Item -Destination '$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources' -Force"
        VERBATIM
    )
endif()

install(
    TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
    )

install(DIRECTORY resources DESTINATION ".")
