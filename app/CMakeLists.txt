# Find vcpkg packages
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED COMPONENTS dll filesystem system function)

# Add source files for the executable
add_executable(MySFMLApp main.cpp ModuleManager.cpp)
target_link_libraries(
    MySFMLApp
    PRIVATE
    fmt::fmt
    spdlog::spdlog
    Boost::dll
    Boost::filesystem
    Boost::system
    Boost::function
)

# Include directories
target_include_directories(
    MySFMLApp
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/interface>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/pingpong_game>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/interface>
)

# Link the GameManager library and SFML libraries
# target_link_libraries(MySFMLApp PRIVATE sfml-graphics sfml-window sfml-system)
target_set_warnings(
    TARGET MySFMLApp ENABLE ${ENABLE_WARNINGS} AS_ERRORS ${ENABLE_WARNINGS_AS_ERRORS}
)

if(WIN32)
    install(
        FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/mysfmlapp.ico DESTINATION resources
        COMPONENT Resources
    )

    install(
        CODE "
        execute_process(
            COMMAND powershell.exe -File ${CMAKE_CURRENT_SOURCE_DIR}/resources/create_shortcuts.ps1 -InstallDir \${CMAKE_INSTALL_PREFIX} -AppName MySFMLApp -ExePath \${CMAKE_INSTALL_PREFIX}/bin/MySFMLApp.exe -IconPath \${CMAKE_INSTALL_PREFIX}/resources/mysfmlapp.ico
            )
            "
    )
endif(WIN32)

install(
    TARGETS MySFMLApp
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
)

set_directory_properties(PROPERTIES VS_STARTUP_PROJECT MySFMLApp)
