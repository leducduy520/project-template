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

add_custom_command(
    TARGET MySFMLApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/create_shortcuts.ps1 ${CMAKE_BINARY_DIR}/create_shortcuts.ps1
)

install(CODE "
    execute_process(
        COMMAND powershell.exe -File \${CMAKE_SOURCE_DIR}/create_shortcuts.ps1 -InstallDir \${CMAKE_INSTALL_PREFIX} -AppName MyApp -ExePath \${CMAKE_INSTALL_PREFIX}/bin/MySFMLApp.exe
    )
")

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