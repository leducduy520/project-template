project(Catch-test)

file(READ ${CMAKE_SOURCE_DIR}/deps.json DEPS_JSON_STRING)

string(JSON DEPS_LENGTH LENGTH ${DEPS_JSON_STRING})
message(STATUS "DEPS_LENGTH ${DEPS_LENGTH}")

foreach(ITR RANGE(${DEPS_LENGTH} - 1))
       string(JSON
              PKG_LABEL
              GET
              ${DEPS_JSON_STRING}
              0
              label
       )
       string(JSON
              PKG_REPO
              GET
              ${DEPS_JSON_STRING}
              0
              repo
       )
       string(JSON
              PKG_BRANCH
              GET
              ${DEPS_JSON_STRING}
              0
              branch
       )

       build_external_project(
              BRANCH
              ${PKG_BRANCH}
              LABEL
              ${PKG_LABEL}
              REPO
              ${PKG_REPO}
       )
endforeach()

add_executable(${PROJECT_NAME} test.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE base)

load_cache(${EXTERNAL_BINARY_DIR} READ_WITH_PREFIX EXTERNAL_ CATCH2_BINARY_DIR)
load_cache(${EXTERNAL_BINARY_DIR} READ_WITH_PREFIX EXTERNAL_ CATCH2_SOURCE_DIR)
load_cache(${EXTERNAL_BINARY_DIR} READ_WITH_PREFIX EXTERNAL_ CATCH2_INSTALL_PREFIX)

load_cache(${EXTERNAL_CATCH2_BINARY_DIR} READ_WITH_PREFIX CATCH2_ PKGCONFIG_INSTALL_DIR)
load_cache(${EXTERNAL_CATCH2_BINARY_DIR} READ_WITH_PREFIX CATCH2_ CATCH_CMAKE_CONFIG_DESTINATION)

list(APPEND CMAKE_PREFIX_PATH ${EXTERNAL_CATCH2_INSTALL_PREFIX}/${CATCH2_CATCH_CMAKE_CONFIG_DESTINATION})
find_package(Catch2 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2WithMain)

list(APPEND CMAKE_MODULE_PATH ${EXTERNAL_CATCH2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}
       RUNTIME PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
       CONFIGURATIONS Debug Release
       DESTINATION ${CMAKE_INSTALL_BINDIR}
       COMPONENT Runtime
       PRIVATE_HEADER PERMISSIONS WORLD_READ OWNER_WRITE
       DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
       COMPONENT PrivateHeader
       PUBLIC_HEADER PERMISSIONS WORLD_READ OWNER_WRITE
       COMPONENT PublicHeader
       DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
