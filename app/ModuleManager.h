#ifndef MODULE_MANAGER_HPP
#define MODULE_MANAGER_HPP

#include <string>
#include <vector>
#include <memory>
#include <boost/dll/shared_library.hpp>
#include <boost/dll/import.hpp>
#include <boost/function.hpp>
#include <boost/filesystem.hpp>
#include <spdlog/spdlog.h>

class ModuleManager {
public:
    // Constructor: Initialize without a fixed directory
    ModuleManager();

    // Destructor: Unload all libraries
    ~ModuleManager();

    // Load a plugin from a user-specified path
    bool load_plugin(const boost::filesystem::path& plugin_path);

    // Unload a specific plugin by name
    bool unload_plugin(const std::string& plugin_name);

    // Check if a plugin is loaded
    bool has_plugin(const std::string& plugin_name) const;

    // Get a function from a specific plugin by name
    template<typename Signature>
    boost::function<Signature> get_function(const std::string& plugin_name,
                                           const std::string& func_name) const;

    // Get all loaded plugin names
    std::vector<std::string> get_plugin_names() const;

private:
    struct Plugin {
        std::shared_ptr<boost::dll::shared_library> library;
        std::string name;
    };

    std::vector<Plugin> plugins_;
};

template<typename Signature>
boost::function<Signature> ModuleManager::get_function(const std::string& plugin_name,
                                                      const std::string& func_name) const {
    for (const auto& plugin : plugins_) {
        if (plugin.name == plugin_name) {
            try {
                return boost::dll::import_symbol<Signature>(*plugin.library, func_name);
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
