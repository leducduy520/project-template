# Add source files for the executable
add_executable(MySFMLApp main.cpp)

# Include directories
target_include_directories(MySFMLApp PRIVATE 
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/game2)

# Link the GameManager library and SFML libraries
target_link_libraries(MySFMLApp GameManager sfml-graphics sfml-window sfml-system)

set_target_properties(MySFMLApp PROPERTIES 
    WINDOWS_EXPORT_ALL_SYMBOLS ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/bin)

install(TARGETS MySFMLApp
    RUNTIME
        DESTINATION  ${CMAKE_INSTALL_BINDIR}
        COMPONENT Runtime
    ARCHIVE
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT Development
    LIBRARY
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT Development
)