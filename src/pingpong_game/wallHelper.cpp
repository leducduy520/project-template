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
        const auto row_count = doc.GetRowCount();
        for(int i = 0; i < row_count;i++)
        {
            const auto rows = doc.GetRow<int>(i);
            for (int j = 0; j < rows.size(); j++)
            {
                float x = constants::brick_padding + j * constants::brick_width;
                float y = i * constants::brick_height;
                w.emplace_back(x, y);
            }
        }
    }
}

