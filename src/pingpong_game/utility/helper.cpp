#include "helper.hpp"
#include "brick.hpp"
#include "magic_enum/magic_enum.hpp"
#include "rapidcsv.h"
#include <chrono>
#include <fmt/format.h>

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

        // int getRandomInt(
        //     std::function<int(std::uniform_int_distribution<int>&, std::mt19937&)>& fnc)
        // {
        //     return fnc(generator::uniform_int_dist, generator::gen);
        // }

        // bool getRandomBoolen(std::function<bool(std::bernoulli_distribution&, std::mt19937&)>& fnc)
        // {
        //     return fnc(generator::bernoulli_dist, generator::gen);
        // }
    } // namespace random

} // namespace utilities
