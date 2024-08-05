#ifndef __WALL_HELPER__
#define __WALL_HELPER__

#include "brick.hpp"
#include "interactions.hpp"
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <mutex>

namespace utilities
{
    namespace wallhelper
    {
        void resetPoint(wall &a_wall);
        void increasePoint(wall &a_wall, uint16_t amount = 1);
        uint16_t getPoint(wall &a_wall);
        void createWall(wall &a_wall, const char *path);
        void destroyAround(wall &a_wall, brick &a_brick, sf::Vector2i range);
        void checkAlive(wall &a_wall);
        template <typename T>
        void interactionwith(wall &a_wall, entity &entity)
        {
            auto elm = dynamic_cast<T *>(&entity);
            if (elm)
            {
                for (auto it = a_wall.begin(); it != a_wall.end(); ++it)
                {
                    interactions::handle_interaction(a_wall, *elm, *(it->second.get()));
                }
            }
            else
            {
                std::cerr << "The type of input entity must be derieved from entity\n";
            }
        }
    } // namespace wallhelper
    namespace texthelper
    {
        class aligning
        {
            static std::unique_ptr<aligning> m_instance;
            static std::once_flag m_flag;
        public:
            enum Alignment{
                TL, TC, TR,
                ML, MC, MR,
                BL, BC, BR
            };
            void operator()(sf::Text* text, const sf::Rect<float>& bound, const Alignment& alignment =  MC);
            static aligning* getInstance()
            {
                std::call_once(m_flag, [&](){
                    m_instance = std::make_unique<aligning>();
                });
                return m_instance.get();
            }
        };
        
    } // namespace texthelper

} // namespace utilities

#endif // __WALL_HELPER__
