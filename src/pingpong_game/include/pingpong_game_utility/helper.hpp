#ifndef __WALL_HELPER__
#define __WALL_HELPER__

#include "brick.hpp"
#include <random>
#include <mutex>

namespace utilities
{
    namespace wallhelper
    {
        void build_wall(wall& a_wall, const char* path);
        void destroy_around(brick& a_brick, sf::Vector2i range);

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
            ROBOTO_REGULAR,
            ROBOTO_ITALIC,
            ROBOTO_BOLD,
            MODESTICSANS_BOLD,
            MODESTICSANS_BOLDITALIC,
        };

        sf::Font& getFont(Style style);
    } // namespace texthelper

    namespace random
    {
        class generator
        {
            static std::random_device rand_device;
            static std::mt19937 gen;
            static std::bernoulli_distribution bernoulli_dist;
            static std::uniform_int_distribution<int> uniform_int_dist;

        public:
            static std::mutex rd_mutex;

            template <class T>
            static T getRandomInt(std::function<T(std::uniform_int_distribution<int>&, std::mt19937&)> fnc)
            {
                std::lock_guard<std::mutex> lock(rd_mutex); // Thread-safe access
                return fnc(uniform_int_dist, gen);          // Use the provided function to generate the random integer
            }

            template <class T>
            static T getRandomBoolean(std::function<T(std::bernoulli_distribution&, std::mt19937&)> fnc)
            {
                std::lock_guard<std::mutex> lock(rd_mutex); // Thread-safe access
                return fnc(bernoulli_dist, gen);            // Use the provided function to generate the random boolean
            }
        };

    } // namespace random

} // namespace utilities

#endif // __WALL_HELPER__
