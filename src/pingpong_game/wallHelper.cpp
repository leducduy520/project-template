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

                std::cout << "hit_point.x " << hit_point.x << ", hit_point.y " << hit_point.y << std::endl;
                w[hit_point];
                //if (w[hit_point].get())
                //{
                //    std::cout << "existing" << std::endl;
                //    //w[hit_point]->hit(constants::cap_brick_hit);
                //}
            }
        }
        std::cout << std::endl;
    }
} // namespace wall_utils
