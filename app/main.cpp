#include <filesystem>
#ifdef _WIN32
#include <Windows.h>
#else
#include <linux/limits.h>
#include <unistd.h>
#include <getopt.h>  // For getopt_long
#endif

#include "IGame.h"
#include "ModuleManager.hpp"
#include <iostream>
#include <cstdlib>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

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

#define SELF_PATH getExecutablePath()

void set_log_file(std::string_view log_file) {
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
    spdlog::set_default_logger(spdlog::basic_logger_mt("basic_logger", log_file.data()));
    spdlog::flush_on(spdlog::level::critical);
    spdlog::flush_every(std::chrono::seconds(5));
}

void set_log_console() {
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
    spdlog::set_default_logger(spdlog::stdout_logger_mt("console_logger"));
    spdlog::flush_on(spdlog::level::critical);
    spdlog::flush_every(std::chrono::seconds(5));
}

void set_log_both(std::string_view log_file) {
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file.data(), true));
    auto logger = std::make_shared<spdlog::logger>("multi_sink", sinks.begin(), sinks.end());
    spdlog::set_default_logger(logger);
    logger->set_level(spdlog::level::debug);
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
    logger->flush_on(spdlog::level::critical);
    spdlog::flush_every(std::chrono::seconds(5));
}

void set_verbose_environment(bool verbose) {
    char env_verbose_disable[8] = "VERBOSE";
    char env_verbose_enable[10] = "VERBOSE=1";
    if (verbose) {
        putenv(env_verbose_enable);
        spdlog::set_level(spdlog::level::trace);
    }
    else {
        putenv(env_verbose_disable);
#ifdef NDEBUG
        spdlog::set_level(spdlog::level::warn);
#else
        spdlog::set_level(spdlog::level::debug);
#endif
    }
}

int main(int argc, char* argv[])
{
    bool verbose = false;
    std::string log_file;
    bool log_console = false;

#ifndef _WIN32
    // Define long options
    static struct option long_options[] = {
        {"verbose",    no_argument,       0, 'v'},
        {"log-file",   required_argument, 0, 'f'},
        {"log-console", no_argument,      0, 'c'},
        {"help",       no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    int c;
    
    // Parse arguments
    while ((c = getopt_long(argc, argv, "vf:ch", long_options, &option_index)) != -1) {
        switch (c) {
        case 'v':
            verbose = true;
            break;
        case 'f':
            log_file = optarg;
            break;
        case 'c':
            log_console = true;
            break;
        case 'h':
            std::cout << "Usage: " << argv[0] << " [options]\n"
                      << "Options:\n"
                      << "  -v, --verbose       Enable verbose logging\n"
                      << "  -f, --log-file PATH Log to file\n"
                      << "  -c, --log-console   Log to console\n"
                      << "  -h, --help          Show this help\n";
            return 0;
        case '?':
            // getopt_long already printed an error message
            return 1;
        default:
            return 1;
        }
    }
#else
    // Simple argument parsing for Windows
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        }
        else if ((arg == "-f" || arg == "--log-file") && i + 1 < argc) {
            log_file = argv[++i];
        }
        else if (arg == "-c" || arg == "--log-console") {
            log_console = true;
        }
        else if (arg == "-h" || arg == "--help") {
            std::cout << "Usage: " << argv[0] << " [options]\n"
                      << "Options:\n"
                      << "  -v, --verbose       Enable verbose logging\n"
                      << "  -f, --log-file PATH Log to file\n"
                      << "  -c, --log-console   Log to console\n"
                      << "  -h, --help          Show this help\n";
            return 0;
        }
        else {
            std::cerr << "Error: Unknown argument: " << arg << "\n";
            return 1;
        }
    }
#endif
    
    if (optind >= argc) {
        spdlog::error("Expected argument after options");
        return EXIT_FAILURE;
    }
    
    // Configure logging
    if (!log_file.empty() && log_console) {
        set_log_both(log_file);
    }
    else if (!log_file.empty()) {
        set_log_file(log_file);
    }
    else if (log_console) {
        set_log_console();
    }
    
    set_verbose_environment(verbose);

#ifdef _WIN32
    ModuleManager moduleManager(SELF_PATH);
#else
    ModuleManager moduleManager(SELF_PATH.parent_path() / "lib");
#endif
    moduleManager.load_plugin("pingpong_game");
    if (moduleManager.has_plugin("pingpong_game")) {
        // Retrieve function pointers for creating and destroying the Game object
        auto createGame = moduleManager.get_function<IGame*()>("pingpong_game", "createPingPongGame");

        if (createGame) {
            // Create the Game object
            auto game = std::unique_ptr<IGame>(createGame());
            if (game) {
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
