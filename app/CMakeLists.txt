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

foreach(build_type IN LISTS CMAKE_CONFIGURATION_TYPES)
    string(TOUPPER ${build_type} build_type_upper)
    set_target_properties(MySFMLApp PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${build_type_upper} ${CMAKE_BINARY_DIR}/${build_type}
        LIBRARY_OUTPUT_DIRECTORY_${build_type_upper} ${CMAKE_BINARY_DIR}/${build_type}/lib
        ARCHIVE_OUTPUT_DIRECTORY_${build_type_upper} ${CMAKE_BINARY_DIR}/${build_type}/lib
    )
endforeach()

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