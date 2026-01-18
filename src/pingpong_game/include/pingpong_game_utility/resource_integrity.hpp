#ifndef RESOURCE_INTEGRITY_H
#define RESOURCE_INTEGRITY_H

#include <filesystem>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

// Use picosha2 for SHA-256 hashing (header-only, available via vcpkg)
#include <picosha2.h>

namespace resource_integrity
{

/**
 * @brief Resource integrity checker that validates files against a manifest
 * Uses SHA-256 hashing to verify file integrity
 */
class ResourceIntegrityChecker
{
public:
    /**
     * @brief Calculate SHA-256 hash of a file
     * @param file_path Path to the file
     * @return Hex string representation of the SHA-256 hash
     */
    static std::string calculate_file_hash(const std::filesystem::path& file_path)
    {
        std::ifstream file(file_path, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Cannot open file for hashing: " + file_path.string());
        }
        std::ostringstream oss;
        oss << file.rdbuf();

        std::vector<unsigned char> hash(picosha2::k_digest_size);
        picosha2::hash256(oss.str(), hash);

        std::string out;
        picosha2::bytes_to_hex_string(hash.begin(), hash.end(), out);
        return out;
    }

    /**
     * @brief Load manifest file containing expected file hashes
     * @param manifest_path Path to the manifest JSON file
     * @return true if manifest loaded successfully, false otherwise
     */
    static bool load_manifest(const std::filesystem::path& manifest_path)
    {
        if (!std::filesystem::exists(manifest_path)) {
            spdlog::warn("Manifest file not found: {}", manifest_path.string());
            return false;
        }

        try {
            std::ifstream file(manifest_path);
            if (!file) {
                spdlog::error("Cannot open manifest file: {}", manifest_path.string());
                return false;
            }

            nlohmann::json manifest;
            file >> manifest;

            manifest_.clear();
            if (manifest.contains("files") && manifest["files"].is_object()) {
                for (const auto& [filename, file_data] : manifest["files"].items()) {
                    if (file_data.is_object() && file_data.contains("hash")) {
                        manifest_[filename] = file_data["hash"].get<std::string>();
                    }
                }
            }

            spdlog::info("Loaded manifest with {} file entries", manifest_.size());
            return true;
        }
        catch (const std::exception& e) {
            spdlog::error("Failed to load manifest: {}", e.what());
            return false;
        }
    }

    /**
     * @brief Verify a file against the manifest
     * @param file_path Path to the file to verify (relative to resources directory)
     * @param resources_base_path Base path to resources directory
     * @return true if file is valid, false if invalid or not in manifest
     */
    static bool verify_file(const std::filesystem::path& file_path,
                            const std::filesystem::path& resources_base_path)
    {
        // Get relative path from resources directory
        std::filesystem::path full_path = resources_base_path / file_path;
        std::filesystem::path relative_path = std::filesystem::relative(full_path, resources_base_path);

        std::string filename = relative_path.string();
        std::replace(filename.begin(), filename.end(), '\\', '/'); // Normalize path separators

        // Check if file is in manifest
        auto it = manifest_.find(filename);
        if (it == manifest_.end()) {
            spdlog::warn("File not in manifest (skipping verification): {}", filename);
            return true; // Allow files not in manifest (optional verification)
        }

        // Calculate file hash
        std::string calculated_hash;
        try {
            calculated_hash = calculate_file_hash(full_path);
        }
        catch (const std::exception& e) {
            spdlog::error("Failed to calculate hash for {}: {}", filename, e.what());
            return false;
        }

        // Compare hashes (case-insensitive comparison)
        std::string expected_hash = it->second;
        std::transform(calculated_hash.begin(), calculated_hash.end(), calculated_hash.begin(), ::tolower);
        std::transform(expected_hash.begin(), expected_hash.end(), expected_hash.begin(), ::tolower);

        if (calculated_hash != expected_hash) {
            spdlog::error("File integrity check FAILED for {}: expected {}, got {}", filename,
                          expected_hash, calculated_hash);
            return false;
        }

        spdlog::debug("File integrity check passed for {}", filename);
        return true;
    }

    /**
     * @brief Generate manifest from resources directory
     * @param resources_path Path to resources directory
     * @param output_path Path where to save the manifest JSON file
     * @return true if manifest generated successfully
     */
    static bool generate_manifest(const std::filesystem::path& resources_path,
                                   const std::filesystem::path& output_path)
    {
        if (!std::filesystem::exists(resources_path) || !std::filesystem::is_directory(resources_path)) {
            spdlog::error("Resources directory does not exist: {}", resources_path.string());
            return false;
        }

        nlohmann::json manifest;
        manifest["version"] = "1.0";
        manifest["generated"] = std::chrono::duration_cast<std::chrono::seconds>(
                                    std::chrono::system_clock::now().time_since_epoch())
                                    .count();
        manifest["files"] = nlohmann::json::object();

        try {
            // Recursively iterate through resources directory
            for (const auto& entry : std::filesystem::recursive_directory_iterator(resources_path)) {
                if (entry.is_regular_file()) {
                    std::filesystem::path file_path = entry.path();
                    std::filesystem::path relative_path = std::filesystem::relative(file_path, resources_path);
                    std::string filename = relative_path.string();
                    std::replace(filename.begin(), filename.end(), '\\', '/');

                    try {
                        std::string hash = calculate_file_hash(file_path);
                        manifest["files"][filename]["hash"] = hash;
                        manifest["files"][filename]["size"] = std::filesystem::file_size(file_path);
                        spdlog::info("Added to manifest: {} (hash: {})", filename, hash);
                    }
                    catch (const std::exception& e) {
                        spdlog::warn("Skipping file {}: {}", filename, e.what());
                    }
                }
            }

            // Write manifest to file
            std::ofstream out_file(output_path);
            if (!out_file) {
                spdlog::error("Cannot write manifest to: {}", output_path.string());
                return false;
            }

            out_file << manifest.dump(2); // Pretty print with 2-space indent
            spdlog::info("Manifest generated successfully: {} ({} files)", output_path.string(),
                         manifest["files"].size());
            return true;
        }
        catch (const std::exception& e) {
            spdlog::error("Failed to generate manifest: {}", e.what());
            return false;
        }
    }

    /**
     * @brief Check if manifest is loaded
     */
    static bool is_manifest_loaded()
    {
        return !manifest_.empty();
    }

private:
    static std::unordered_map<std::string, std::string> manifest_;
};

// Static member definition
inline std::unordered_map<std::string, std::string> ResourceIntegrityChecker::manifest_{};

} // namespace resource_integrity

#endif // RESOURCE_INTEGRITY_H
