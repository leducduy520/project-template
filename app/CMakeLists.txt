# Add source files for the executable
add_executable(MySFMLApp main.cpp)

# Include directories
target_include_directories(MySFMLApp PRIVATE 
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/game2)

# Link the GameManager library and SFML libraries
target_link_libraries(MySFMLApp PRIVATE pingpong_game sfml-graphics sfml-window sfml-system)

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