project(app)

add_library(base SHARED base.cpp)
target_include_directories(base PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

build_external_project(
    FIRST_INDEX 0
    LAST_INDEX 0
    LABEL "yaml"
    REPO "https://github.com/jbeder/yaml-cpp.git"
    TAG "0.8.0"
    SHALLOW "ON"
    CMAKE_AGRS "\"-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>\" \"-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}\" \"-DYAML_BUILD_SHARED_LIBS=ON\""
)

list(APPEND CMAKE_PREFIX_PATH "${EXTERNAL_DIR}/EP_BASE/Install/yaml")
find_package(yaml-cpp REQUIRED)
target_link_libraries(base PUBLIC yaml-cpp::yaml-cpp)

add_executable(app main.cpp)
target_link_libraries(app PRIVATE base)

install(TARGETS app base)

install(IMPORTED_RUNTIME_ARTIFACTS yaml-cpp::yaml-cpp)


