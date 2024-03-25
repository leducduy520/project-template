project(base_include)

file(GLOB HEADER_FILES CONFIGURE_DEPENDS "*.hpp" "*.h")

target_sources(base PRIVATE ${HEADER_FILES})

set_target_properties(base PROPERTIES PRIVATE_HEADER ${HEADER_FILES})

target_include_directories(
    base PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}
    )

install(TARGETS base
        RUNTIME PERMISSIONS WORLD_READ OWNER_WRITE OWNER_EXECUTE
                CONFIGURATIONS "Debug" "Release"
                DESTINATION ${CMAKE_INSTALL_BINDIR}
                COMPONENT Runtime
        ARCHIVE PERMISSIONS WORLD_READ OWNER_WRITE OWNER_EXECUTE
                CONFIGURATIONS "Debug" "Release"
                DESTINATION ${CMAKE_INSTALL_LIBDIR}
                COMPONENT Development
        PRIVATE_HEADER PERMISSIONS WORLD_READ OWNER_WRITE
                       DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
                       COMPONENT PrivateHeader
        PUBLIC_HEADER PERMISSIONS WORLD_READ OWNER_WRITE
                      COMPONENT PublicHeader
                      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        )
