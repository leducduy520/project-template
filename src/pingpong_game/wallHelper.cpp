#include "wallHelper.hpp"
#include "rapidcsv.h"

namespace wall_utils{
    void createWall(wall &w, const char *path)
    {
        if(w.size())
        {
            w.clear();
        }
        rapidcsv::Document doc(path, rapidcsv::LabelParams(-1, -1));
        const auto padding = (constants::window_width - constants::brick_width * doc.GetColumnCount()) / 2;
        for(int i = 0; i < doc.GetRowCount();i++)
        {
            const auto rows = doc.GetRow<int>(i);
            for (int j = 0; j < rows.size(); j++)
            {
                float x = padding + j * constants::brick_width;
                float y = i * constants::brick_height;
                brick::BrickProperty property = static_cast<brick::BrickProperty>(doc.GetCell<int>(j , i));
                w[{x, y}] = std::make_unique<brick>(x, y, property);
            }
        }
    }
    void destroyAround(wall& w, brick& br, sf::Vector2i range)
    {
        
    }
    } // namespace wall_utils
