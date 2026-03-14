# Resource manifest generator tool
project(generate_manifest)

# Dependencies
find_path(PICOSHA2_INCLUDE_DIRS "picosha2.h")
find_package(nlohmann_json CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

# Executable
add_executable(generate_manifest
    generate_manifest.cpp
)

# Includes
target_include_directories(
    generate_manifest
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/../utility/include
        ${PICOSHA2_INCLUDE_DIRS}
)

# Links
target_link_libraries(
    generate_manifest
    PRIVATE
        nlohmann_json::nlohmann_json
        spdlog::spdlog_header_only
)

# Compiler options
if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU|Clang")
    target_compile_options(generate_manifest PRIVATE -Wall -Wextra)
endif()

# Properties
set_target_properties(
    generate_manifest
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tools
)

# Post-build command
add_custom_command(
    TARGET generate_manifest
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Generating resource manifest..."
    COMMAND $<TARGET_FILE:generate_manifest>
            "${CMAKE_CURRENT_SOURCE_DIR}"
            "${CMAKE_CURRENT_SOURCE_DIR}/resources_manifest.json"
    COMMENT "Generating resource integrity manifest"
    VERBATIM
)

# Resource files
set(RESOURCE_FILES
    wall.csv
    resources_manifest.json
    background.jpg
    ball.png
    brick.png
    steel.png
    bomb_explosion.wav
    brick_bounce.wav
    diamond_destroy.wav
    game_background.wav
    paddle_bounce.wav
    wall_bounce.wav
    Cross-Boxed.ttf
    ModesticSans/ModesticSans-Bold.ttf
    ModesticSans/ModesticSans-BoldItalic.ttf
    Roboto/Roboto-Bold.ttf
    Roboto/Roboto-Italic.ttf
    Roboto/Roboto-Regular.ttf
    PARENT_SCOPE
)

source_group("Resource Files" FILES ${RESOURCE_FILES})

# Install
install(
    DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    COMPONENT Resources
    PATTERN "*.cpp" EXCLUDE
    PATTERN "*.h" EXCLUDE
    PATTERN "*.hpp" EXCLUDE
    PATTERN "CMakeLists.txt" EXCLUDE
)
