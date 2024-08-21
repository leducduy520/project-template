#include "helper.hpp"
#include "brick.hpp"
#include "magic_enum.hpp"
#include "rapidcsv.h"
#include <exception>
#include <ios>

namespace utilities
{
    namespace wallhelper
    {
        void resetPoint(wall& a_wall)
        {
            a_wall.point = 0;
        }

        uint16_t getPoint(wall& a_wall)
        {
            return a_wall.point;
        }

        void increasePoint(wall& a_wall, uint16_t amount)
        {
            a_wall.point += amount;
        }

        void createWall(wall& a_wall, const char* path)
        {
            if (!a_wall.empty())
            {
                a_wall.clear();
            }
            resetPoint(a_wall);

            const rapidcsv::Document doc(path, rapidcsv::LabelParams(-1, -1));

            const auto padding = (constants::window_width - constants::brick_width * doc.GetColumnCount()) / 2;
            for (decltype(doc.GetRowCount()) i = 0; i < doc.GetRowCount(); i++)
            {
                const auto rows = doc.GetRow<int>(i);
                for (decltype(rows.size()) j = 0; j < rows.size(); j++)
                {
                    const auto px_x = (padding + j * constants::brick_width) * 1.0F;
                    const auto px_y = (i * constants::brick_height) * 1.0F;
                    const auto property = static_cast<brick::BrickProperty>(doc.GetCell<int>(j, i));
                    auto a_brick = std::make_unique<brick>(px_x, px_y, property);
                    a_brick->registerLiveUpdate(
                        [&a_wall](bool is_increase) mutable { a_wall.updateLive(is_increase); });
                    a_wall.emplace(
                        std::make_pair<const e_location, std::unique_ptr<brick>>({px_x, px_y}, std::move(a_brick)));
                }
            }
        }

        void destroyAround(wall& a_wall, brick& a_brick, sf::Vector2i range)
        {
            const sf::Vector2f explode_point{a_brick.x(), a_brick.y()};

            auto px_x = explode_point.x - (range.x - 1) / 2 * a_brick.width();
            auto px_y = explode_point.y - (range.y - 1) / 2 * a_brick.height();

            for (int i = 0; i < range.y; ++i)
            {
                for (int j = 0; j < range.x; ++j)
                {
                    const sf::Vector2f hit_point{px_x + i * a_brick.width(), px_y + j * a_brick.height()};

                    auto iter = a_wall.find(hit_point);
                    if (iter != a_wall.end() && !iter->second->is_destroyed())
                    {
                        auto* alias = iter->second.get();
                        if (alias->getProperty() == brick::BOMB)
                        {
                            alias->hit(constants::cap_brick_hit);
                            destroyAround(a_wall, *alias, range);
                        }
                        else
                        {
                            if (alias->getProperty() == brick::DIAMOND && !alias->is_destroyed())
                            {
                                utilities::wallhelper::increasePoint(a_wall, 5);
                            }
                            else if (alias->getProperty() == brick::BRICK && !alias->is_destroyed())
                            {
                                utilities::wallhelper::increasePoint(a_wall, 1);
                            }
                            alias->hit(constants::cap_brick_hit, true);
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
    } // namespace texthelper
} // namespace utilities
