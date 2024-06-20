#include <filesystem>
#ifdef _WIN32
    #include <Windows.h>
#else
    #include <unistd.h>
    #include <linux/limits.h>
#endif

#include "PingPongGame.hpp"

std::filesystem::path getExecutablePath() {
#ifdef _WIN32
    char path[MAX_PATH];
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    return std::filesystem::path(path).parent_path();
#else
    char path[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
    return std::filesystem::path(std::string(path, (count > 0) ? count : 0)).parent_path();
#endif
}

#define EXECUTABLE_PATH (getExecutablePath())

int main() {

    auto execPath = EXECUTABLE_PATH;
    PingPongGame gameManager((execPath / "resources/").string());
    gameManager.run();
    return 0;
}
