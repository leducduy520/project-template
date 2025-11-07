#include <filesystem>
#ifdef _WIN32
#include <Windows.h>
#else
#include <linux/limits.h>
#include <unistd.h>
#endif

#include "IGame.h"
#include "ModuleManager.hpp"

static boost::filesystem::path getExecutablePath()
{
#ifdef _WIN32
    char path[MAX_PATH];
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    return boost::filesystem::path(path).parent_path();
#else
    char path[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
    return std::filesystem::path(std::string(path, (count > 0) ? count : 0)).parent_path();
#endif
}

#define EXECUTABLE_PATH getExecutablePath()

int main()
{
    ModuleManager moduleManager(EXECUTABLE_PATH);
    const std::string plugin_name = "pingpong_game";
    
    try {
        moduleManager.load_plugin(plugin_name);
    } catch (const std::exception& e) {
        // Plugin loading failed
        return 1;
    }
    
    if (moduleManager.has_plugin(plugin_name))
    {
        // Retrieve function pointers for creating and destroying the Game object
        using CreateGameFunc = IGame* (*)();
        using DestroyGameFunc = void (*)(IGame*);
        auto createGame = moduleManager.get_function<IGame*()>(plugin_name, "createPingPongGame");
        auto destroyGame = moduleManager.get_function<void(IGame*)>(plugin_name, "createPingPongGame");
        
        if (createGame && destroyGame)
        {
            // Create the Game object
            IGame* game = createGame();
            if (game)
            {
                std::string path = (getExecutablePath().parent_path() / "resources").string();
                game->init(path);
                game->run();
                // Destroy the Game object
                destroyGame(game);
            }
        }

        // Release module
        moduleManager.unload_plugin(plugin_name);
    }
    return 0;
}
