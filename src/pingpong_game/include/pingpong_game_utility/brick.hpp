#ifndef _BRICK_H
#define _BRICK_H

#include "constants.hpp"
#include "entity.hpp"
#include <functional>
#include <limits>

#define fnhit(OBJECT) void hit_##OBJECT(bool& destroyed, const bool relate) const;

class wall;

class brick : public Ientity
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

    brick() = default;
    brick(float x, float y, BrickProperty property = BRICK);
    BrickProperty getProperty() const noexcept;
    wall* getWall() const noexcept;
    void draw(sf::RenderWindow& window) noexcept override;
    void hit(const int damage = 1, const bool relate = false) noexcept;
    void init(float x, float y) noexcept override;
    void update() noexcept override;
    void registerDiamondAmountCallback(const std::function<void(int)>& fnc);
    void registerPontUpdate(const std::function<void(int16_t)>& fnc) noexcept;
    void registerParent(wall* parent) noexcept;
    friend class wall;

private:
    wall* m_wall;
    BrickProperty m_property;
    int m_hitCount;
    std::function<void(int)> m_diamond_amount_update_fnc;
    std::function<void(int16_t)> m_point_update_fnc;
    static sf::Texture& get_texture(BrickProperty property = BRICK);
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

using WallMap = std::map<e_location, std::unique_ptr<brick>> ;

struct WallStatus
{
    uint16_t point = 0;
    int live = 0;
};

class wall : public Ientity
{
    WallMap m_data;
    WallStatus m_status;
public:
    wall() = default;

    // Brick management methods
    WallMap& data() noexcept { return m_data; }
    bool empty() const noexcept { return m_data.empty(); }
    std::size_t size() const noexcept { return m_data.size(); }
    void clear() noexcept { m_data.clear(); }

    // Status get/set methods
    const WallStatus& getStatus() const noexcept { return m_status; }
    void setStatus(const WallStatus& status) noexcept { m_status = status; }
    
    // Render and update methods
    void update() override;
    void draw(sf::RenderWindow& window) noexcept override;
    void init(float x, float y) override;
    void refresh();
};
#endif // _BRICK_H
