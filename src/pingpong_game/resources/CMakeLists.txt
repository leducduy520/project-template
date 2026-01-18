# =============================================================================
# Resource Manifest Generator Tool
# =============================================================================
# This tool generates a manifest file with SHA-256 hashes for all resource files
# to enable integrity checking at runtime.

project(generate_manifest)

# =============================================================================
# Find Dependencies
# =============================================================================
find_path(PICOSHA2_INCLUDE_DIRS "picosha2.h")
find_package(nlohmann_json CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

# =============================================================================
# Executable Definition
# =============================================================================
add_executable(generate_manifest
    generate_manifest.cpp
)

# =============================================================================
# Include Directories
# =============================================================================
target_include_directories(
    generate_manifest
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/pingpong_game_utility
        ${PICOSHA2_INCLUDE_DIRS}
)

# =============================================================================
# Link Libraries
# =============================================================================
target_link_libraries(
    generate_manifest
    PRIVATE
        nlohmann_json::nlohmann_json
        spdlog::spdlog_header_only
)

# =============================================================================
# Compiler Options
# =============================================================================
if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU|Clang")
    target_compile_options(generate_manifest PRIVATE -Wall -Wextra)
endif()

# =============================================================================
# Set Output Directory
# =============================================================================
# Output the executable to the build directory, not installed
set_target_properties(
    generate_manifest
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tools
)

# =============================================================================
# Post-Build: Generate Resource Manifest
# =============================================================================
# Generate resource manifest after building generate_manifest tool
# The manifest will be generated in the source resources directory
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

# =============================================================================
# Install Resource Files
# =============================================================================
# Install all resource files to install_prefix/resources
# Exclude *.cpp and CMakeLists.txt files
install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION resources
    COMPONENT Resources
    PATTERN "*.cpp" EXCLUDE
    PATTERN "CMakeLists.txt" EXCLUDE
)
