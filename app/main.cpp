#include <filesystem>
#ifdef _WIN32
#include <Windows.h>
#else
#include <linux/limits.h>
#include <unistd.h>
#endif

#include "IGame.h"
#include "ModuleManager.h"
#include <boost/filesystem.hpp>

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
    ModuleManager moduleManager;
    moduleManager.load_plugin(EXECUTABLE_PATH / "pingpong_game.dll");
    if (moduleManager.has_plugin("pingpong_game.dll"))
    {
        boost::function<IGame*()> CreateGameFunc = moduleManager.get_function<IGame*()>("pingpong_game.dll", "createGame");
        boost::function<void(IGame*)> DestroyGameFunc = moduleManager.get_function<void(IGame*)>("pingpong_game.dll", "destroyGame");
        if (CreateGameFunc && DestroyGameFunc)
        {
            // Create the Game object
            IGame* game = CreateGameFunc();
            if (game)
            {
                std::string path = (getExecutablePath().parent_path() / "resources/").string();
                game->init(path);
                game->run();
                // Destroy the Game object
                DestroyGameFunc(game);
            }
        }

        // Release module
        moduleManager.unload_plugin("pingpong_game.dll");
    }
    return 0;
}
