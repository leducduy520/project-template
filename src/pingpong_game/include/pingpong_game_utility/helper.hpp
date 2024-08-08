#ifndef __WALL_HELPER__
#define __WALL_HELPER__

#include "brick.hpp"

namespace utilities
{
    namespace wallhelper
    {
        void resetPoint(wall& a_wall);
        void increasePoint(wall& a_wall, uint16_t amount = 1);
        void createWall(wall& a_wall, const char* path);
        void destroyAround(wall& a_wall, brick& a_brick, sf::Vector2i range);

    } // namespace wallhelper

    namespace texthelper
    {
        class aligning
        {
        public:
            enum Alignment
            {
                TL,
                TC,
                TR,
                ML,
                MC,
                MR,
                BL,
                BC,
                BR
            };

            static void Aligning(sf::Text* text, const sf::Rect<float>& bound, const Alignment& alignment = MC);
        };

    } // namespace texthelper

} // namespace utilities

#endif // __WALL_HELPER__
