#include "helper.hpp"
#include "brick.hpp"
#include "magic_enum.hpp"
#include "rapidcsv.h"
#include <exception>
#include <ios>
#include <filesystem>
#include <regex>

template void wall::updatePoint<short>(short&& amount) noexcept;

namespace utilities
{
    namespace wallhelper
    {
        void buildWall(wall& a_wall, const char* path)
        {
            if (!a_wall.empty())
            {
                a_wall.clear();
            }
            a_wall.resetPoint();

            const rapidcsv::Document doc(path, rapidcsv::LabelParams(-1, -1));

            const auto padding = (constants::window_width - constants::brick_width * doc.GetColumnCount()) / 2;
            for (decltype(doc.GetRowCount()) i = 0; i < doc.GetRowCount(); i++)
            {
                const auto rows = doc.GetRow<int>(i);
                for (decltype(rows.size()) j = 0; j < rows.size(); j++)
                {
                    const auto px_x = static_cast<float>(padding + j * constants::brick_width);
                    const auto px_y = static_cast<float>(i * constants::brick_height);
                    const auto property = static_cast<brick::BrickProperty>(doc.GetCell<int>(j, i));
                    auto a_brick = std::make_unique<brick>(px_x, px_y, property);
                    a_brick->registerLiveUpdate(
                        [&a_wall](bool is_increase) mutable { a_wall.updateLive(is_increase); });
                    a_brick->registerPontUpdate([&a_wall](short point) mutable { a_wall.updatePoint(point); });
                    a_brick->registerParent(&a_wall);
                    a_wall.emplace(
                        std::make_pair<const e_location, std::unique_ptr<brick>>({px_x, px_y}, std::move(a_brick)));
                }
            }
        }

        void destroyAround(brick& a_brick, sf::Vector2i range)
        {
            const sf::Vector2f explode_point{a_brick.x(), a_brick.y()};
            wall* a_wall = a_brick.getWall();

            auto px_x = explode_point.x - floorf((static_cast<float>(range.x) - 1.0F) / 2.0F) * a_brick.width();
            auto px_y = explode_point.y - floorf((static_cast<float>(range.y) - 1.0F) / 2.0F) * a_brick.height();

            for (int i = 0; i < range.y; ++i)
            {
                for (int j = 0; j < range.x; ++j)
                {
                    const sf::Vector2f hit_point{px_x + static_cast<float>(i) * a_brick.width(),
                                                 px_y + static_cast<float>(j) * a_brick.height()};

                    auto iter = a_wall->find(hit_point);
                    if (iter != a_wall->end() && !iter->second->is_destroyed())
                    {
                        auto* alias = iter->second.get();
                        alias->hit(constants::cap_brick_hit, true);
                        if (alias->getProperty() == brick::BOMB)
                        {
                            destroyAround(*alias, range);
                        }
                    }
                }
            }
        }
    } // namespace wallhelper

    namespace texthelper
    {
        void aligning::Aligning(sf::Text* text, const sf::Rect<float>& bound, const aligning::Alignment& alignment)
        {
            text->setOrigin({0, 0});
            const auto text_width = text->getGlobalBounds().width;
            const auto text_height = text->getGlobalBounds().height;
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
                // for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{constants::resoucesPath})
                // {
                //     if(std::regex_search(dir_entry.path().string().c_str(), match, txt_regex))
                //     {
                //         std::cout << dir_entry << '\n';
                //     }
                // }

                Font::font_cross_boxed.loadFromFile(constants::resoucesPath + "Cross Boxed.ttf");
                Font::font_modesticsans_bolditalic.loadFromFile(constants::resoucesPath +
                                                                "ModesticSans/ModesticSans-BoldItalic.ttf");
                Font::font_modesticsans_bold.loadFromFile(constants::resoucesPath +
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
