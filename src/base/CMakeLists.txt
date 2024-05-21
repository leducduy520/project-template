project(${BASE_NAME})

file(GLOB SRC_FILES CONFIGURE_DEPENDS "*.cpp" "*.c")
file(GLOB HEADER_FILES CONFIGURE_DEPENDS "*.hpp" "*.h")

add_library(${PROJECT_NAME} SHARED ${SRC_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER ${HEADER_FILES})

target_include_directories(
    ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

build_external_project(
    FIRST_INDEX
    "0"
    LAST_INDX
    "1"
    REPO
    "https://github.com/benhoyt/inih.git"
    "https://github.com/p-ranav/csv2.git"
    LABEL
    "IniReader"
    "CSV2"
    TAG
    "master"
    "master"
    SHALLOW
    "ON"
    "ON"
    CONFIGURE_COMMAND
    "meson setup ./ ${EXTERNAL_DIR}/EP_BASE/Source/IniReader --prefix <INSTALL_DIR>"
    "OFF"
    CMAKE_AGRS
    "OFF"
    "\"-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>\" \"-DCMAKE_BUILD_TYPE=@CMAKE_BUILD_TYPE@\""
    BUILD_COMMAND
    "meson compile"
    "OFF"
    INSTALL_COMMAND
    "meson install"
    "OFF"
    )

set(ENV{PKG_CONFIG_PATH} "${EXTERNAL_DIR}/EP_BASE/Install/IniReader/lib/pkgconfig")
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBINI REQUIRED IMPORTED_TARGET INIReader)
target_link_libraries(${PROJECT_NAME} PUBLIC PkgConfig::LIBINI)

list(APPEND CMAKE_PREFIX_PATH "${EXTERNAL_DIR}/EP_BASE/Install/CSV2")
find_package(csv2 REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC csv2::csv2)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
    RUNTIME CONFIGURATIONS "Debug" "Release" DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
    ARCHIVE CONFIGURATIONS "Debug" "Release" DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT PublicHeader
    )

install(EXPORT ${PROJECT_NAME}-targets DESTINATION ${INSTALL_CONFIG_DIR} NAMESPACE ZooClass::)

# install(IMPORTED_RUNTIME_ARTIFACTS ${PROJECT_NAME} RUNTIME_DEPENDENCY_SET _deps)
# install(RUNTIME_DEPENDENCY_SET _deps
#     PRE_INCLUDE_REGEXES "lib[Ii][Nn][Ii]"
#     PRE_EXCLUDE_REGEXES "ext-ms-*" "api-ms-*" "[Aa][Zz][Uu][Rr][Ee]" "libstdc"
#     POST_INCLUDE_REGEXES ".*external/.*\\.dll"
#     POST_EXCLUDE_REGEXES ".*system32/.*\\.dll"
#     DIRECTORIES "${EXTERNAL_DIR}/EP_BASE/Install/IniReader/lib" "${EXTERNAL_DIR}/EP_BASE/Install/IniReader/bin" "${EXTERNAL_DIR}/EP_BASE/Install/IniReader"
# )
