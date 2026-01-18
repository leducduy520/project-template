#include "resource_integrity.hpp"
#include <iostream>
#include <filesystem>
#include <spdlog/spdlog.h>

int main(int argc, char* argv[])
{
    // Configure spdlog for console output
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("[%^%l%$] %v");

    // Parse command line arguments
    std::filesystem::path resources_path;
    std::filesystem::path output_path;

    if (argc == 3) {
        resources_path = argv[1];
        output_path = argv[2];
    }
    else if (argc == 2) {
        // If only one argument, assume it's the resources path
        // and output to resources_manifest.json in the same directory
        resources_path = argv[1];
        output_path = std::filesystem::path(resources_path) / "resources_manifest.json";
    }
    else if (argc == 1) {
        // No arguments: use current directory as resources path
        resources_path = std::filesystem::current_path();
        output_path = resources_path / "resources_manifest.json";
    }
    else {
        std::cerr << "Usage: " << argv[0] << " [resources_path] [output_path]\n";
        std::cerr << "  resources_path: Path to resources directory (default: current directory)\n";
        std::cerr << "  output_path: Path where to save manifest (default: resources_path/resources_manifest.json)\n";
        std::cerr << "\n";
        std::cerr << "Examples:\n";
        std::cerr << "  " << argv[0] << " /path/to/resources /path/to/manifest.json\n";
        std::cerr << "  " << argv[0] << " /path/to/resources\n";
        std::cerr << "  " << argv[0] << "\n";
        return 1;
    }

    // Validate resources path
    if (!std::filesystem::exists(resources_path)) {
        spdlog::error("Resources directory does not exist: {}", resources_path.string());
        return 1;
    }

    if (!std::filesystem::is_directory(resources_path)) {
        spdlog::error("Path is not a directory: {}", resources_path.string());
        return 1;
    }

    // Convert to absolute paths
    resources_path = std::filesystem::absolute(resources_path);
    output_path = std::filesystem::absolute(output_path);

    spdlog::info("Generating manifest for resources in: {}", resources_path.string());
    spdlog::info("Output manifest file: {}", output_path.string());

    // Generate manifest
    if (resource_integrity::ResourceIntegrityChecker::generate_manifest(resources_path, output_path)) {
        spdlog::info("Manifest generated successfully!");
        return 0;
    }
    else {
        spdlog::error("Failed to generate manifest");
        return 1;
    }
}
