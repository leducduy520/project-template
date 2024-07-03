#include "wallHelper.hpp"
#include "rapidcsv.h"
#include <exception>
#include <ios>

namespace wall_utils{
    void createWall(wall &a_wall, const char *path)
    {
        if(!a_wall.empty())
        {
            a_wall.clear();
        }

        const rapidcsv::Document doc(path, rapidcsv::LabelParams(-1, -1));

        const auto padding = (constants::window_width - constants::brick_width * doc.GetColumnCount()) / 2;
        for (decltype(doc.GetRowCount()) i = 0; i < doc.GetRowCount(); i++)
        {
            const auto rows = doc.GetRow<int>(i);
            for (decltype(rows.size()) j = 0; j < rows.size(); j++)
            {
                const float px_x = padding + j * constants::brick_width;
                const float px_y = i * constants::brick_height;
                const auto property = static_cast<brick::BrickProperty>(doc.GetCell<int>(j, i));
                a_wall[{px_x, px_y}] = std::make_unique<brick>(px_x, px_y, property);
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
                if (iter != a_wall.end() && !iter->second.get()->is_destroyed())
                {
                    auto* alias = iter->second.get();
                    if(alias->getProperty() == brick::BOMB)
                    {
                        alias->hit(constants::cap_brick_hit);
                        destroyAround(a_wall, *alias, range);
                    }
                    else
                    {
                        alias->hit(constants::cap_brick_hit, true);
                    }
                }
            }
        }
    }
    void checkAlive(wall &w)
    {
        bool is_destroyed = true;
        for (auto it = w.rbegin(); it != w.rend();)
        {
            if (it->second->is_destroyed())
            {
                w.erase(it->first);
            }
            else
            {
                if (is_destroyed)
                {
                    if (it->second.get()->getProperty() == brick::DIAMOND)
                        is_destroyed = false;
                }
                ++it;
            }
        }

        if (is_destroyed)
        {
            w.destroy();
        }
    }
} // namespace wall_utils
