project(Catch-test)

build_external_project(READ_FROM_JSON)

list(APPEND CMAKE_PREFIX_PATH ${EXTERNAL_INSTALL_PREFIX})

add_executable(${PROJECT_NAME} test.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE base)

find_package(Catch2 REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2WithMain)

load_cache(${EXTERNAL_BINARY_DIR} READ_WITH_PREFIX EXTERNAL_ catch2_SOURCE_DIR)
list(APPEND CMAKE_MODULE_PATH ${EXTERNAL_catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(${PROJECT_NAME})

install(
    TARGETS ${PROJECT_NAME}
    RUNTIME CONFIGURATIONS Debug Release
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT Runtime
    PRIVATE_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT PrivateHeader
    PUBLIC_HEADER COMPONENT PublicHeader DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
