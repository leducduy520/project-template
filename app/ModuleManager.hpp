#ifndef MODULE_MANAGER_HPP
#define MODULE_MANAGER_HPP

#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <functional>
#include <stdexcept>
#include <boost/dll/shared_library.hpp>
#include <boost/dll/import.hpp>
#include <boost/filesystem.hpp>
#include <spdlog/spdlog.h>

struct Plugin {
    std::shared_ptr<boost::dll::shared_library> library;
    std::string name;
    Plugin() noexcept = default;
};

class ModuleManager {
public:
    // Constructor: Initialize with plugin directory
    explicit ModuleManager(const boost::filesystem::path& plugin_dir);

    ModuleManager(const ModuleManager&) = delete;
    ModuleManager& operator=(const ModuleManager&) = delete;
    ModuleManager(ModuleManager&& other) noexcept 
        : plugin_dir_(std::move(other.plugin_dir_)),
        plugins_(std::move(other.plugins_))
    {
    }
    ModuleManager& operator=(ModuleManager&& other) noexcept {
        if (this != &other) {
            plugin_dir_ = std::move(other.plugin_dir_);
            plugins_ = std::move(other.plugins_);
        }
        return *this;
    }

    // Destructor: Unload all libraries
    ~ModuleManager();

    // Load a specific plugin by name (without extension)
    void load_plugin(const std::string& plugin_name);

    // Unload a specific plugin by name
    bool unload_plugin(const std::string& plugin_name);

    // Check if a plugin is loaded
    bool has_plugin(const std::string& plugin_name) const;

    // Get a function from a specific plugin by name
    template<typename Signature>
    std::function<Signature> get_function(const std::string& plugin_name,
                                         const std::string& func_name) const;

    // Get all loaded plugin names
    std::vector<std::string> get_plugin_names() const;

    // Get all available plugin names in the plugin directory (without extension)
    std::vector<std::string> list_available_plugins() const;

private:
    boost::filesystem::path plugin_dir_;
    std::vector<Plugin> plugins_;
};

template<typename Signature>
std::function<Signature> ModuleManager::get_function(const std::string& plugin_name,
                                                     const std::string& func_name) const {
    for (const auto& plugin : plugins_) {
        if (plugin.name == plugin_name) {
            try {
                auto imported_func = boost::dll::import_symbol<Signature>(*plugin.library, func_name);
                return std::function<Signature>(imported_func);
            } catch (const std::exception& e) {
                spdlog::error("Failed to import '{}' from plugin '{}': {}",
                              func_name, plugin_name, e.what());
                throw;
            }
        }
    }
    spdlog::error("Plugin '{}' not found", plugin_name);
    throw std::runtime_error("Plugin not found: " + plugin_name);
}

#endif // MODULE_MANAGER_HPP
