# Add source files for the executable
add_executable(MySFMLApp main.cpp ModuleManager.cpp)

# Include directories
target_include_directories(MySFMLApp PRIVATE 
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/interface>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/game2>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/interface>)

# Link the GameManager library and SFML libraries
target_link_libraries(MySFMLApp PRIVATE sfml-graphics sfml-window sfml-system)

set_target_properties(
    MySFMLApp PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>"
)

install(TARGETS MySFMLApp
    RUNTIME
        DESTINATION  "."
        COMPONENT Runtime
    ARCHIVE
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT Development
    LIBRARY
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT Development
)