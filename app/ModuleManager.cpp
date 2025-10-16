#include "ModuleManager.h"
#include <boost/filesystem.hpp>
#include <spdlog/spdlog.h>

ModuleManager::ModuleManager() {
    spdlog::info("ModuleManager initialized");
}

ModuleManager::~ModuleManager() {
    spdlog::info("Unloading all plugins");
    plugins_.clear();
}

bool ModuleManager::load_plugin(const boost::filesystem::path& plugin_path) {
    namespace fs = boost::filesystem;
    const std::string ext =
#ifdef _WIN32
        ".dll";
#else
        ".so";
#endif

    if (plugin_path.extension() != ext) {
        spdlog::error("Invalid plugin extension for {}: expected {}", plugin_path.string(), ext);
        return false;
    }

    std::string plugin_name = plugin_path.filename().string();
    if (has_plugin(plugin_name)) {
        spdlog::warn("Plugin {} already loaded", plugin_name);
        return false;
    }

    spdlog::info("Attempting to load plugin: {}", plugin_path.string());
    try {
        Plugin plugin;
        plugin.name = plugin_name;
        plugin.library = std::make_shared<boost::dll::shared_library>(plugin_path);
        plugins_.push_back(std::move(plugin));
        spdlog::info("Successfully loaded plugin: {}", plugin_name);
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Failed to load plugin {}: {}", plugin_path.string(), e.what());
        return false;
    }
}

bool ModuleManager::unload_plugin(const std::string& plugin_name) {
    auto it = std::find_if(plugins_.begin(), plugins_.end(),
        [&plugin_name](const Plugin& p) { return p.name == plugin_name; });
    if (it != plugins_.end()) {
        spdlog::info("Unloading plugin: {}", plugin_name);
        plugins_.erase(it);
        return true;
    }
    spdlog::warn("Plugin {} not found for unloading", plugin_name);
    return false;
}

bool ModuleManager::has_plugin(const std::string& plugin_name) const {
    return std::any_of(plugins_.begin(), plugins_.end(),
        [&plugin_name](const Plugin& p) { return p.name == plugin_name; });
}

std::vector<std::string> ModuleManager::get_plugin_names() const {
    std::vector<std::string> names;
    for (const auto& plugin : plugins_) {
        names.push_back(plugin.name);
    }
    return names;
}
