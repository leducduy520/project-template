#include "wallHelper.hpp"
#include "rapidcsv.h"
#include <exception>
#include <ios>

namespace wall_utils{
    void createWall(wall &w, const char *path)
    {
        if(w.size())
        {
            w.clear();
        }

        rapidcsv::Document doc(path, rapidcsv::LabelParams(-1, -1));

        const auto padding = (constants::window_width - constants::brick_width * doc.GetColumnCount()) / 2;
        for (int i = 0; i < doc.GetRowCount(); i++)
        {
            const auto rows = doc.GetRow<int>(i);
            for (int j = 0; j < rows.size(); j++)
            {
                float x = padding + j * constants::brick_width;
                float y = i * constants::brick_height;
                brick::BrickProperty property = static_cast<brick::BrickProperty>(doc.GetCell<int>(j, i));
                w[{x, y}] = std::make_unique<brick>(x, y, property);
            }
        }
    }

    void destroyAround(wall& w, brick& br, sf::Vector2i range)
    {
        sf::Vector2f explode_point{br.x(), br.y()};
        
        auto x = explode_point.x - (range.x - 1) / 2 * br.width();
        auto y = explode_point.y - (range.y - 1) / 2 * br.height();

        for (int i = 0; i < range.y; ++i)
        {
            for (int j = 0; j < range.x; ++j)
            {
                sf::Vector2f hit_point{x + i * br.width(), y + j * br.height()};

                auto it = w.find(hit_point);
                if (it != w.end() && !it->second.get()->is_destroyed())
                {
                    auto alias = it->second.get();
                    alias->hit(constants::cap_brick_hit);
                    if(alias->getProperty() == brick::BOMB)
                    {
                        destroyAround(w, *alias, range);
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
