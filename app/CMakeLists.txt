project(app)

add_library(base SHARED base.cpp)
target_include_directories(
    base PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

build_external_project(
    LABEL
    "yaml"
    "wise_enum"
    REPO
    "https://github.com/jbeder/yaml-cpp.git"
    "https://github.com/quicknir/wise_enum.git"
    TAG
    "0.8.0"
    "3.1.0"
    SHALLOW
    "ON"
    "ON"
    CMAKE_ARGS_MORE
    "\"-DYAML_BUILD_SHARED_LIBS=ON\""
    "OFF"
    )

find_package(yaml-cpp REQUIRED)
target_link_libraries(base PUBLIC yaml-cpp::yaml-cpp)

find_package(WiseEnum REQUIRED)
target_link_libraries(base PUBLIC WiseEnum::wise_enum)

target_link_libraries(base PRIVATE pprint)

add_executable(app main.cpp)
target_link_libraries(app PRIVATE base)

install(TARGETS app base)

install(IMPORTED_RUNTIME_ARTIFACTS yaml-cpp::yaml-cpp)
