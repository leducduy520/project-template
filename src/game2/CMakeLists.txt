# Add source files for the library
add_library(GameManager STATIC GameManager.cpp Utils.cpp entity.cpp ball.cpp background.cpp paddle.cpp brick.cpp)

# Include directories
target_include_directories(GameManager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)

# Find SFML
# find_package(SFML COMPONENTS graphics window system REQUIRED)

# Link SFML libraries
target_link_libraries(GameManager sfml-graphics sfml-window sfml-system)


set_target_properties(GameManager PROPERTIES 
    WINDOWS_EXPORT_ALL_SYMBOLS ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/lib)