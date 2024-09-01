#ifndef __WALL_HELPER__
#define __WALL_HELPER__

#include "brick.hpp"
#include <random>
#include <mutex>

namespace utilities
{
    namespace wallhelper
    {
        void buildWall(wall& a_wall, const char* path);
        void destroyAround(brick& a_brick, sf::Vector2i range);

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

        /// @brief Convert time_t duration in second to a string presents for digital clock
        /// @param duration
        /// @return a formated string "MM:SS"
        std::string format_duration(time_t duration);

        enum Style
        {
            CROSS_BOXED,
            MODESTICSANS_BOLD,
            MODESTICSANS_BOLDITALIC,
        };

        sf::Font& getFont(Style style);

        class Font
        {
            static sf::Font font_cross_boxed;
            static sf::Font font_modesticsans_bold;
            static sf::Font font_modesticsans_bolditalic;

        public:
            friend sf::Font& getFont(Style style);
        };
    } // namespace texthelper

    namespace random
    {
        extern std::random_device rd;
        extern std::mt19937 gen;
        extern std::bernoulli_distribution bernoulli_dist;
        extern std::uniform_int_distribution<int> uniform_int_dist;
        extern std::mutex rd_mutex;
    } // namespace random

} // namespace utilities

#endif // __WALL_HELPER__
