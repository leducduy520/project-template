#include "helper.hpp"
#include "brick.hpp"
#include "magic_enum.hpp"
#include "rapidcsv.h"
#include <exception>
#include <ios>
#include <filesystem>
#include <regex>

template void wall::update_point<short>(short&& amount) noexcept;

namespace utilities
{
    namespace wallhelper
    {
        void destroy_around(brick& a_brick, const sf::Vector2i range)
        {

        }
    } // namespace wallhelper

    namespace texthelper
    {
        void aligning::Aligning(sf::Text* text, const FloatRect& bound, const aligning::Alignment& alignment)
        {
            text->setOrigin({0, 0});
            const auto text_width = text->getGlobalBounds().size.x;
            const auto text_height = text->getGlobalBounds().size.y;
            sf::Vector2f newpos{};

            const auto align_name = magic_enum::enum_name(alignment);
            switch (align_name[1])
            {
            case 'L':
                newpos.x = bound.left;
                break;
            case 'C':
                newpos.x = bound.left + (bound.width - text_width) / 2;
                break;
            case 'R':
                newpos.x = bound.left + bound.width - text_width;
                break;
            default:
                throw std::invalid_argument{"Invalid alignment direction"};
            }
            switch (align_name[0])
            {
            case 'B':
                newpos.y = bound.top + bound.height - text_height;
                break;
            case 'M':
                newpos.y = bound.top + (bound.height - text_height) / 2;
                break;
            case 'T':
                newpos.y = bound.top;
                break;
            default:
                throw std::invalid_argument{"Invalid alignment direction"};
            }
            text->setPosition(newpos);
        }

        std::string format_duration(time_t duration)
        {
            // Calculate minutes and seconds
            const auto minutes = duration / 60;
            const auto seconds = duration % 60;

            // Create a stringstream to format the string
            std::stringstream strstream;
            strstream << std::setw(2) << std::setfill('0') << minutes << ":" << std::setw(2) << std::setfill('0')
                      << seconds;

            return strstream.str();
        }

        sf::Font& getFont(Style style)
        {
            static bool initialized = false;
            if (!initialized)
            {
                // std::cout << "\nAll fonts in resource path:\n";
                // const std::regex txt_regex("[a-z]+\\.ttf");
                // std::cmatch match;
                // for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{constants::resouces_path})
                // {
                //     if(std::regex_search(dir_entry.path().string().c_str(), match, txt_regex))
                //     {
                //         std::cout << dir_entry << '\n';
                //     }
                // }

                Font::font_cross_boxed.openFromFile(constants::resouces_path + "Cross Boxed.ttf");
                Font::font_modesticsans_bolditalic.openFromFile(constants::resouces_path +
                                                                "ModesticSans/ModesticSans-BoldItalic.ttf");
                Font::font_modesticsans_bold.openFromFile(constants::resouces_path +
                                                          "ModesticSans/ModesticSans-Bold.ttf");
                initialized = true;
            }

            switch (style)
            {
            case Style::CROSS_BOXED:
                return Font::font_cross_boxed;
            case Style::MODESTICSANS_BOLD:
                return Font::font_modesticsans_bold;
            case Style::MODESTICSANS_BOLDITALIC:
                return Font::font_modesticsans_bolditalic;
            }
            return Font::font_cross_boxed;
        }

        sf::Font Font::font_cross_boxed{};
        sf::Font Font::font_modesticsans_bold{};
        sf::Font Font::font_modesticsans_bolditalic{};
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
