#include "helper.hpp"
#include "brick.hpp"
#include "magic_enum/magic_enum.hpp"
#include "rapidcsv.h"
#include "resource_integrity.hpp"
#include <chrono>
#include <fmt/format.h>
#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <sys/resource.h>
#include <unistd.h>
#endif
#include <spdlog/spdlog.h>

namespace utilities
{
    namespace wallhelper
    {
        void destroy_around(brick& a_brick, sf::Vector2f range)
        {
            const sf::Vector2f explode_point{a_brick.x(), a_brick.y()};
            auto neighbors = a_brick.get_neighbors(range);

            for (auto& neighbor : neighbors) {
                neighbor->hit(constants::cap_brick_hit, true);
                if (neighbor->getProperty() == brick::BOMB) {
                    destroy_around(*neighbor, range);
                }
            }
        }
    } // namespace wallhelper

    namespace texthelper
    {
        void aligning::Aligning(sf::Text* text, const sf::Rect<float>& bound, const aligning::Alignment& alignment)
        {
            text->setOrigin({0, 0});
            const auto text_width = text->getGlobalBounds().size.x;
            const auto text_height = text->getGlobalBounds().size.y;
            sf::Vector2f newpos{};

            const auto align_name = magic_enum::enum_name(alignment);
            switch (align_name[1]) {
            case 'L':
                newpos.x = bound.position.x;
                break;
            case 'C':
                newpos.x = bound.position.x + (bound.size.x - text_width) / 2;
                break;
            case 'R':
                newpos.x = bound.position.x + bound.size.x - text_width;
                break;
            default:
                throw std::invalid_argument{"Invalid alignment direction"};
            }
            switch (align_name[0]) {
            case 'B':
                newpos.y = bound.position.y + bound.size.y - text_height;
                break;
            case 'M':
                newpos.y = bound.position.y + (bound.size.y - text_height) / 2;
                break;
            case 'T':
                newpos.y = bound.position.y;
                break;
            default:
                throw std::invalid_argument{"Invalid alignment direction"};
            }
            text->setPosition(newpos);
        }

        std::string format_duration(time_t duration)
        {
            using namespace std::chrono;

            const auto total_seconds = seconds(duration);
            const auto mins = duration_cast<minutes>(total_seconds).count();
            const auto secs = (total_seconds % minutes(1)).count();

            return fmt::format("{:02d}:{:02d}", mins, secs);
        }

        sf::Font& getFont(Style style)
        {
            static bool initialized = false;

            static sf::Font font_cross_boxed;
            static sf::Font font_roboto_regurlar;
            static sf::Font font_roboto_italic;
            static sf::Font font_roboto_bold;
            static sf::Font font_modesticsans_bold;
            static sf::Font font_modesticsans_bolditalic;

            if (!initialized) {
                if (!resource_integrity::ResourceIntegrityChecker::is_manifest_loaded()) {
                    resource_integrity::ResourceIntegrityChecker::load_manifest(constants::resouces_path / "resources_manifest.json");
                }
                auto resource_check = [](const std::string& filename) {
                    if (!resource_integrity::ResourceIntegrityChecker::verify_file(filename, constants::resouces_path)) {
                        throw std::logic_error("File integrity check FAILED for " + filename);
                    }
                };
                resource_check("Cross Boxed.ttf");
                resource_check("Roboto/Roboto-Regular.ttf");
                resource_check("Roboto/Roboto-Italic.ttf");
                resource_check("Roboto/Roboto-Bold.ttf");
                resource_check("ModesticSans/ModesticSans-BoldItalic.ttf");
                resource_check("ModesticSans/ModesticSans-Bold.ttf");


                (void)font_cross_boxed.openFromFile((constants::resouces_path / "Cross Boxed.ttf").string());
                (void)font_roboto_regurlar.openFromFile((constants::resouces_path / "Roboto/Roboto-Regular.ttf").string());
                (void)font_roboto_italic.openFromFile((constants::resouces_path / "Roboto/Roboto-Italic.ttf").string());
                (void)font_roboto_bold.openFromFile((constants::resouces_path / "Roboto/Roboto-Bold.ttf").string());
                (void)font_modesticsans_bolditalic.openFromFile(
                    (constants::resouces_path / "ModesticSans/ModesticSans-BoldItalic.ttf").string());
                (void)font_modesticsans_bold.openFromFile(
                    (constants::resouces_path / "ModesticSans/ModesticSans-Bold.ttf").string());
                initialized = true;
            }

            switch (style) {
            case Style::CROSS_BOXED:
                return font_cross_boxed;
            case Style::ROBOTO_REGULAR:
                return font_roboto_regurlar;
            case Style::ROBOTO_ITALIC:
                return font_roboto_italic;
            case Style::ROBOTO_BOLD:
                return font_roboto_bold;
            case Style::MODESTICSANS_BOLD:
                return font_modesticsans_bold;
            case Style::MODESTICSANS_BOLDITALIC:
                return font_modesticsans_bolditalic;
            }
            return font_cross_boxed;
        }
    } // namespace texthelper

    namespace random
    {
        std::random_device generator::rand_device{};
        std::mt19937 generator::gen(rand_device());
        std::bernoulli_distribution generator::bernoulli_dist(0.25F);
        std::uniform_int_distribution<int> generator::uniform_int_dist(-10, 10);
        std::mutex generator::rd_mutex{};
    } // namespace random

    namespace profiling
    {
        bool enabled = false;
        std::chrono::steady_clock::time_point start_time;

        void init()
        {
            if (enabled) {
                start_time = std::chrono::steady_clock::now();
                spdlog::info("Profiling enabled");
            }
        }

        void log_stats()
        {
            if (!enabled) return;

            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count();

            long memory_kb = 0;
            double cpu_time = 0.0;

#ifdef _WIN32
            PROCESS_MEMORY_COUNTERS pmc;
            if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
                memory_kb = pmc.WorkingSetSize / 1024;
            }

            FILETIME creation, exit, kernel, user;
            if (GetProcessTimes(GetCurrentProcess(), &creation, &exit, &kernel, &user)) {
                ULARGE_INTEGER k, u;
                k.LowPart = kernel.dwLowDateTime;
                k.HighPart = kernel.dwHighDateTime;
                u.LowPart = user.dwLowDateTime;
                u.HighPart = user.dwHighDateTime;
                cpu_time = (k.QuadPart + u.QuadPart) / 10000000.0; // Convert 100ns to seconds
            }
#else
            struct rusage usage;
            getrusage(RUSAGE_SELF, &usage);
            memory_kb = usage.ru_maxrss; // in KB on Linux

            cpu_time = usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1e6 +
                       usage.ru_stime.tv_sec + usage.ru_stime.tv_usec / 1e6;
#endif

            spdlog::info("Profiling: Elapsed time: {}s, CPU time: {:.2f}s, Memory: {} KB", elapsed, cpu_time, memory_kb);
        }

        void start_monitoring()
        {
            // Could start a thread for periodic logging, but for simplicity, call log_stats periodically
        }

        void stop_monitoring()
        {
            if (enabled) {
                log_stats();
                spdlog::info("Profiling stopped");
            }
        }
    } // namespace profiling

} // namespace utilities
