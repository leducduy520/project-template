project(app)

add_executable(${PROJECT_NAME} app.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE base)

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