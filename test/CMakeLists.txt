project(test)

add_executable(test test.cpp)
target_link_libraries(test PRIVATE base)
find_package(PkgConfig REQUIRED)

if(PKG_CONFIG_FOUND)
    load_cache(${EXTERNAL_DIR}/build READ_WITH_PREFIX EXTERNAL_ CATCH2_BINARY_DIR)
    load_cache(${EXTERNAL_CATCH2_BINARY_DIR} READ_WITH_PREFIX CATCH2_ PKGCONFIG_INSTALL_DIR)
    load_cache(${EXTERNAL_CATCH2_BINARY_DIR} READ_WITH_PREFIX CATCH2_ CMAKE_INSTALL_PREFIX)
    set(CATCH2_PKGCONFIG_INSTALL_DIR ${CATCH2_CMAKE_INSTALL_PREFIX}/${CATCH2_PKGCONFIG_INSTALL_DIR})
    set(ENV{PKG_CONFIG_PATH} ${CATCH2_PKGCONFIG_INSTALL_DIR})
    pkg_check_modules(CATCH2 REQUIRED Catch2-With-Main)

    target_include_directories(test PRIVATE ${CATCH2_INCLUDE_DIRS})
    target_link_directories(test PRIVATE ${CATCH2_LIBRARY_DIRS})
    string(
        REPLACE ".a"
                ".a;"
                CATCH2_LINK_LIBRARIES
                ${CATCH2_LINK_LIBRARIES}
        )
    message(STATUS "CATCH2_LINK_LIBRARIES ${CATCH2_LINK_LIBRARIES}")
    target_link_libraries(test PRIVATE ${CATCH2_LINK_LIBRARIES})
else()
    list(APPEND CMAKE_PREFIX_PATH ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/cmake/Catch2)
    find_package(Catch2 REQUIRED)
    target_link_libraries(test PRIVATE Catch2::Catch2WithMain)
endif()

install(
    TARGETS ${PROJECT_NAME}
    RUNTIME PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
            CONFIGURATIONS Debug Release
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT Runtime
    PRIVATE_HEADER
        PERMISSIONS WORLD_READ OWNER_WRITE
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT PrivateHeader
    PUBLIC_HEADER
        PERMISSIONS WORLD_READ OWNER_WRITE
        COMPONENT PublicHeader
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
