#include "ModuleManager.hpp"
#include <boost/filesystem.hpp>
#include <spdlog/spdlog.h>

ModuleManager::ModuleManager(const boost::filesystem::path& plugin_dir) : plugin_dir_(plugin_dir)
{
    spdlog::info("ModuleManager initialized with plugin directory: {}", plugin_dir_.string());
    if (!boost::filesystem::exists(plugin_dir_)) {
        spdlog::warn("Plugin directory {} does not exist, creating it", plugin_dir_.string());
        boost::filesystem::create_directory(plugin_dir_);
    }
}

ModuleManager::~ModuleManager()
{
    // Use try-catch to prevent exceptions from destructor
    // spdlog can throw on file I/O errors or format issues
    try {
        spdlog::info("Unloading all plugins");
    }
    catch (...) {
        // Silently ignore logging errors in destructor
        // We can't throw from destructor as it would call std::terminate()
    }
    plugins_.clear();
}

void ModuleManager::load_plugin(const std::string& plugin_name)
{
    namespace fs = boost::filesystem;
    namespace dll = boost::dll;

    // Check if plugin is already loaded
    if (has_plugin(plugin_name)) {
        spdlog::warn("Plugin '{}' is already loaded", plugin_name);
        return;
    }

    // Get platform-specific extension using Boost.DLL
    const std::string ext = dll::shared_library::suffix().string();

    // Construct full plugin path
    fs::path plugin_path = plugin_dir_ / (plugin_name + ext);

    spdlog::info("Attempting to load plugin: {} from {}", plugin_name, plugin_path.string());

    if (!fs::exists(plugin_path)) {
        spdlog::error("Plugin file not found: {}", plugin_path.string());
        throw std::runtime_error("Plugin file not found: " + plugin_path.string());
    }

    try {
        Plugin plugin;
        plugin.name = plugin_name;
        plugin.library = std::make_shared<dll::shared_library>(plugin_path);
        plugins_.push_back(std::move(plugin));
        spdlog::info("Successfully loaded plugin: {}", plugin_name);
    }
    catch (const std::exception& e) {
        spdlog::error("Failed to load plugin {}: {}", plugin_name, e.what());
        throw;
    }
}

bool ModuleManager::unload_plugin(const std::string& plugin_name)
{
    auto it = std::find_if(plugins_.begin(), plugins_.end(), [&plugin_name](const Plugin& p) {
        return p.name == plugin_name;
    });
    if (it != plugins_.end()) {
        spdlog::info("Unloading plugin: {}", plugin_name);
        plugins_.erase(it);
        spdlog::info("Plugin {} unloaded", plugin_name);
        return true;
    }
    spdlog::warn("Plugin {} not found for unloading", plugin_name);
    return false;
}

bool ModuleManager::has_plugin(const std::string& plugin_name) const
{
    return std::any_of(plugins_.begin(), plugins_.end(), [&plugin_name](const Plugin& p) {
        return p.name == plugin_name;
    });
}

std::vector<std::string> ModuleManager::get_plugin_names() const
{
    std::vector<std::string> names;
    for (const auto& plugin : plugins_) {
        names.push_back(plugin.name);
    }
    return names;
}

std::vector<std::string> ModuleManager::list_available_plugins() const
{
    namespace fs = boost::filesystem;
    namespace dll = boost::dll;

    std::vector<std::string> available_plugins;

    // Get platform-specific extension using Boost.DLL
    const std::string ext = dll::shared_library::suffix().string();

    if (!fs::exists(plugin_dir_)) {
        spdlog::warn("Plugin directory {} does not exist", plugin_dir_.string());
        return available_plugins;
    }

    try {
        for (const auto& entry : fs::directory_iterator(plugin_dir_)) {
            if (fs::is_regular_file(entry) && entry.path().extension().string() == ext) {
                // Get filename without extension
                std::string plugin_name = entry.path().stem().string();
                available_plugins.push_back(plugin_name);
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        spdlog::error("Error scanning plugin directory {}: {}", plugin_dir_.string(), e.what());
    }

    return available_plugins;
}
