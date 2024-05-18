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
        "meson setup ./ ${EXTERNAL_DIR}/EP_BASE/Source/IniReader --prefix ${CMAKE_CURRENT_BINARY_DIR}/IniReader"
        "OFF"
    CMAKE_AGRS
        "OFF"
        "\"-DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/csv2\" \"-DCMAKE_BUILD_TYPE=@CMAKE_BUILD_TYPE@\""
    BUILD_COMMAND 
        "meson compile"
        "OFF"
    INSTALL_COMMAND 
        "meson install"
        "OFF"
)

set(ENV{PKG_CONFIG_PATH} "${CMAKE_CURRENT_BINARY_DIR}/IniReader/lib/pkgconfig")
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBINI REQUIRED IMPORTED_TARGET INIReader)
target_link_libraries(${PROJECT_NAME} PkgConfig::LIBINI)

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_BINARY_DIR}/csv2)
find_package(csv2 REQUIRED)
target_link_libraries(${PROJECT_NAME} csv2::csv2)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
    RUNTIME CONFIGURATIONS "Debug" "Release" DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
    ARCHIVE CONFIGURATIONS "Debug" "Release" DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT PublicHeader
    )

install(EXPORT ${PROJECT_NAME}-targets DESTINATION ${INSTALL_CONFIG_DIR} NAMESPACE ZooClass::)
