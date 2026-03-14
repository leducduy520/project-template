# Source directory project
project(src)

# Subdirectories
add_subdirectory(ipc)
add_subdirectory(pingpong_game)

# Install interface headers
install(
    DIRECTORY interface
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT Development
)

# Enable folder organization in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
