#ifndef _BRICK_H
#define _BRICK_H

#include "constants.hpp"
#include "entity.hpp"
#include <functional>
#include <limits>
#include <map>

#define fnhit(OBJECT) void hit_##OBJECT(bool& destroyed, const bool relate) const;

class wall;

class brick : public static_entity
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

    void draw(sf::RenderWindow& window) noexcept override;
    void hit(const int damage = 1, const bool relate = false) noexcept;
    void init(float x, float y) noexcept override;
    void update() noexcept override;

    BrickProperty getProperty() const noexcept;
    std::set<brick*> get_neighbors(sf::Vector2f range) const noexcept;

    void registerDiamondAmountCallback(const std::function<void(int)>& fnc);
    void registerPontUpdate(const std::function<void(int16_t)>& fnc) noexcept;
    void registerParent(wall* parent) noexcept;

    void on_message(const std::string&, Ientity*) override;
    friend class wall;

private:
    wall* m_wall;
    BrickProperty m_property;
    int m_hitCount;

    std::function<void(int)> m_diamond_amount_update_fnc;
    std::function<void(int16_t)> m_point_update_fnc;

    fnhit(brick) fnhit(diamond) fnhit(bomb) fnhit(scaleup) fnhit(clone)

    static sf::Texture& get_texture(BrickProperty property = BRICK);
};

using e_location = sf::Vector2f;

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

using WallMap = std::map<e_location, std::unique_ptr<brick>>;

struct WallStatus
{
    uint16_t point = 0;
    int live = 0;
};

class wall : public static_entity
{
    WallMap m_data;
    WallStatus m_status;

public:
    wall() = default;

    // Brick management methods
    WallMap& data() noexcept
    {
        return m_data;
    }

    bool empty() const noexcept
    {
        return m_data.empty();
    }

    std::size_t size() const noexcept
    {
        return m_data.size();
    }

    void clear() noexcept
    {
        m_data.clear();
    }

    // Status get/set methods
    const WallStatus& getStatus() const noexcept
    {
        return m_status;
    }

    void setStatus(const WallStatus& status) noexcept
    {
        m_status = status;
    }

    std::set<brick*> get_brick_neighbors(const brick& a_brick, sf::Vector2f range) const noexcept;

    // Render and update methods
    void update() override;
    void draw(sf::RenderWindow& window) noexcept override;
    void init(float x, float y) override;
    void load_from_file(std::filesystem::path file);
    void refresh();
    void destroy() noexcept override;
};
#endif // _BRICK_H
