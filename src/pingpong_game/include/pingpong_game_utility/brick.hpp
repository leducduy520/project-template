#ifndef _BRICK_H
#define _BRICK_H

#include "constants.hpp"
#include "entity.hpp"
#include <functional>
#include <memory>
#include <unordered_map>
#include <utility>

class wall;

class brick : public entity
{
public:
    enum BrickProperty
    {
        NONE = 0,
        BRICK,
        DIAMOND,
        BOMB,
        SCALEUP
    };

    brick() = default;
    brick(float x, float y, BrickProperty property = BRICK);
    BrickProperty getProperty() const noexcept;
    void draw(sf::RenderWindow& window) override;
    void hit(const int damage = 1, const bool relate = false) noexcept;
    void init(float x, float y) override;
    void update() override;
    void registerLiveUpdate(const std::function<void(bool)>& fnc);
    friend class wall;

private:
    BrickProperty m_property;
    int m_hitCount;
    std::function<void(bool)> m_live_update_fnc;
    static sf::Texture& getTexture(BrickProperty property = BRICK);
};

extern sf::Image& getImage(brick::BrickProperty property);

typedef sf::Vector2f e_location;

namespace std
{
    template <>
    struct less<e_location>
    {
        bool operator()(const e_location& lhs, const e_location& rhs) const
        {
            return lhs.x < rhs.x ? true : (lhs.x > rhs.x ? false : (lhs.y < rhs.y));
        }
    };
} // namespace std

//// Custom hash function for e_location
//struct PairHash {
//    std::size_t operator()(const e_location& p) const {
//        return std::hash<float>()(p.first) ^ std::hash<float>()(p.second) ^ std::hash<float>()(p.first * p.second) ^ std::hash<float>()(p.first - p.second);
//    }
//};
//
//// Custom equality function for e_location
//struct PairEqual {
//    bool operator()(const e_location& lhs, const e_location& rhs) const {
//        return lhs.first == rhs.first && lhs.second == rhs.second;
//    }
//};

typedef std::map<e_location, std::unique_ptr<brick>> wall_map;

class wall : public wall_map, public entity
{
public:
    wall() = default;

    template <class T>
    wall(T&& bricks) noexcept : point(0), live(0)
    {
        swap(std::forward<T>(bricks));
    }

    void updateLive(bool increase) noexcept;
    void update() override;
    void draw(sf::RenderWindow& window) override;
    void init(float x, float y) override;
    void refresh();

    uint16_t point;
    unsigned int live;

private:
};

#endif // _BRICK_H
