project(Catch-test)

add_executable(${PROJECT_NAME} test.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE base)

target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2WithMain)

set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(${PROJECT_NAME})

install(
    TARGETS ${PROJECT_NAME} RUNTIME CONFIGURATIONS Debug Release DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
    )
