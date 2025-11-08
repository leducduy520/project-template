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
    return boost::filesystem::path(std::string(path, (count > 0) ? count : 0)).parent_path();
#endif
}

#define EXECUTABLE_PATH getExecutablePath()

int main()
{
    #ifdef NDEBUG
        spdlog::set_level(spdlog::level::warn);
    #else
        spdlog::set_level(spdlog::level::debug);
    #endif
#ifdef _WIN32    
    ModuleManager moduleManager(EXECUTABLE_PATH);
#else
    ModuleManager moduleManager(EXECUTABLE_PATH.parent_path()/"lib");
#endif
    moduleManager.load_plugin("pingpong_game");
    if (moduleManager.has_plugin("pingpong_game"))
    {
        // Retrieve function pointers for creating and destroying the Game object
        auto createGame = moduleManager.get_function<IGame*()>("pingpong_game", "createPingPongGame");
        
        if (createGame)
        {
            // Create the Game object
            auto game = std::unique_ptr<IGame>(createGame());
            if (game)
            {
                std::string path = (getExecutablePath().parent_path() / "resources").string();
                game->init(path);
                game->run();
            }
        }

        // Release module
        moduleManager.unload_plugin("pingpong_game");
    }
    return 0;
}
