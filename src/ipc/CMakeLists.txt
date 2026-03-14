project(ipc)

# Find Boost components
find_package(Boost REQUIRED COMPONENTS interprocess)

# IPC Library
add_library(ipc_lib STATIC
    src/IPCManager.cpp
    src/ChildProcess.cpp
    src/MainProcessManager.cpp
)

target_include_directories(ipc_lib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/ipc>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(ipc_lib
    PUBLIC
        Boost::boost
        Boost::interprocess
)

target_set_warnings(
    TARGET ipc_lib
    ENABLE ${ENABLE_WARNINGS}
    AS_ERRORS ${ENABLE_WARNINGS_AS_ERRORS}
)

set_target_properties(ipc_lib PROPERTIES
    FOLDER "IPC"
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)

# Example executables
add_executable(ChildProcessExample
    examples/child_process_example.cpp
)

target_link_libraries(ChildProcessExample
    PRIVATE
        ipc_lib
)

target_set_warnings(
    TARGET ChildProcessExample
    ENABLE ${ENABLE_WARNINGS}
    AS_ERRORS ${ENABLE_WARNINGS_AS_ERRORS}
)

set_target_properties(ChildProcessExample PROPERTIES
    FOLDER "IPC/Examples"
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

add_executable(MainProcessExample
    examples/main_process_example.cpp
)

target_link_libraries(MainProcessExample
    PRIVATE
        ipc_lib
)

target_set_warnings(
    TARGET MainProcessExample
    ENABLE ${ENABLE_WARNINGS}
    AS_ERRORS ${ENABLE_WARNINGS_AS_ERRORS}
)

set_target_properties(MainProcessExample PROPERTIES
    FOLDER "IPC/Examples"
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# Install targets
install(
    TARGETS ipc_lib ChildProcessExample MainProcessExample
    EXPORT ipcTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ipc
    FILES_MATCHING PATTERN "*.hpp"
)
