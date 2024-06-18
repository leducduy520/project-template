# Add source files for the executable
add_executable(MySFMLApp main.cpp)

# Include directories
target_include_directories(MySFMLApp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

# Link the GameManager library and SFML libraries
target_link_libraries(MySFMLApp GameManager sfml-graphics sfml-window sfml-system)

set_target_properties(MySFMLApp PROPERTIES 
    WINDOWS_EXPORT_ALL_SYMBOLS ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/bin)