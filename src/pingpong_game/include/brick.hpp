#ifndef _BRICK_H
#define _BRICK_H

#include "constants.hpp"
#include "entity.hpp"
#include <unordered_map>
#include <utility>
#include <memory>

class brick : public entity
{
public:
    enum BrickProperty
    {
        NONE = 0,
        BRICK,
        DIAMOND,
        BOMB
    };

    brick(float x, float y, BrickProperty property = BRICK);
    brick() = default;

    void init(float x, float y) override;

    // Implement the pure virtual functions
    void update() override;
    void draw(sf::RenderWindow &window) override;

    BrickProperty getProperty() const noexcept;

    void hit(const int damage = 1) noexcept;

    friend class wall;
private:
    BrickProperty m_property;
    int m_hitCount;
    static sf::Texture& getTexture(BrickProperty property = BRICK);

};

typedef sf::Vector2f e_location;

namespace std {
    template<>
    struct less<e_location> {
        bool operator()(const e_location& lhs, const e_location& rhs) const {
            return lhs.x < rhs.x ?  true : (lhs.x > rhs.x ? false : (lhs.y < rhs.y));
        }
    };
}

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
    wall(T&& bricks) noexcept
    {
        swap(std::forward<T>(bricks));
    }
    void update() override;
    void draw(sf::RenderWindow &window) override;
    void init(float x, float y) override;

private:
};

#endif // _BRICK_H
