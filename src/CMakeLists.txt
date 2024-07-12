project(src)

add_subdirectory(pingpong_game)

install(
    DIRECTORY interface DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT PublicHeader
    )
