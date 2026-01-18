project(src)

add_subdirectory(pingpong_game)

install(DIRECTORY interface DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT Development)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
