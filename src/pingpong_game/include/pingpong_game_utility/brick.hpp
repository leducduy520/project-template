#ifndef _BRICK_H
#define _BRICK_H

#include "constants.hpp"
#include "entity.hpp"
#include <functional>
#include <memory>
#include <map>
#include <utility>
#include <type_traits>
#include <limits>
#include <numeric>

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
        SCALEUP,
        CLONE
    };

    enum class HitSource
    {
        FROM_BALL,
        FROM_EXPLOSION
    };

    struct HitInfo
    {
        unsigned int damage;
        HitSource damage_source;
    };

public:    
    brick() = default;
    brick(float x, float y, BrickProperty property = BRICK);
    
    void draw(sf::RenderWindow& window) override;
    void init(float x, float y) override;
    void update() override;
    
    BrickProperty getProperty() const noexcept;
    void hit(HitInfo info) noexcept;
    
    wall* getWall() const noexcept;
    void registerHealthUpdate(const std::function<void(bool)>& fnc);
    void registerPontUpdate(const std::function<void(int16_t)>& fnc);
    void registerParent(wall* parent);
    friend class wall;

private:
    wall* m_wall;
    BrickProperty m_property;
    int m_hit_count;
    std::function<void(bool)> m_live_update_fnc;
    std::function<void(int16_t)> m_point_update_fnc;
    static sf::Texture& get_texture(BrickProperty property = BRICK);
#define fnhit(OBJECT) void hit_##OBJECT(bool& destroyed, const HitSource hit_source);
    fnhit(brick) fnhit(diamond) fnhit(bomb) fnhit(scaleup) fnhit(clone)
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

typedef std::map<e_location, std::unique_ptr<brick>> WallMap;

class wall : public entity
{
    WallMap m_data;
public:
    wall() = default;

    WallMap& data() noexcept { return m_data; }
    bool empty() const noexcept { return m_data.empty(); }
    std::size_t size() const noexcept { return m_data.size(); }
    void clear() noexcept { m_data.clear(); }

    void update_health(bool increase) noexcept;
    void bomb_exploding_at(const e_location& center, const float& radius) noexcept;
    
    template <class T>
    inline void update_point(T&& amount) noexcept;
    uint16_t get_point() const noexcept;
    void reset_point() noexcept;
    
    void update() override;
    void draw(sf::RenderWindow& window) override;
    void init(float x, float y) override;
    void refresh();

private:
    uint16_t point;
    unsigned int live;
};

template <class T>
inline void wall::update_point(T&& amount) noexcept
{
    static_assert(std::is_integral<std::remove_reference_t<T>>::value, "Integral required.");
    point += static_cast<uint16_t>(amount);
}

#endif // _BRICK_H
