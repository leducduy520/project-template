project(Catch-test)

FetchContent_Declare(
    Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git GIT_TAG v3.6.0
    )
FetchContent_MakeAvailable(Catch2)

add_executable(${PROJECT_NAME} test.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE pingpong_game_util)

target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2WithMain)

set_target_properties(
    ${PROJECT_NAME} PROPERTIES INSTALL_RPATH
                               ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
    )

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(${PROJECT_NAME})

install(
    TARGETS ${PROJECT_NAME}
    RUNTIME CONFIGURATIONS Debug Release DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT Runtime
    )
