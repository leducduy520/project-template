#include <cstdlib>
#include <exception>

#include "helper.hpp"
#include "brick.hpp"
#include "magic_enum.hpp"
#include "rapidcsv.h"
#include "SoundManager.hpp"

extern template void wall::update_point<short>(short&& amount) noexcept;

using namespace std::literals;

sf::Image& getImage(brick::BrickProperty property)
{
    static bool initialized = false;
    static std::vector<sf::Image> list;
    if (!initialized)
    {
        sf::Image source;
        if (!source.loadFromFile(constants::resouces_path + "brick.png"))
        {
            const std::string message = "Cannot open source image "s + constants::resouces_path + "brick.png";
            throw std::logic_error(message.c_str());
        }

        const auto width = source.getSize().x;
        const auto height = source.getSize().y;
        const auto* const pixels = source.getPixelsPtr();
        std::vector<std::vector<sf::Uint8>> matrix(width / constants::brick_width);

        for (unsigned int px_y = 0; px_y < height; ++px_y)
        {
            for (unsigned int px_x = 0; px_x < width; ++px_x)
            {
                auto index = (px_x + px_y * width) * 4; // 4 bytes per pixel (RGBA)
                auto index_matrix = px_x / constants::brick_width;
                matrix.at(index_matrix).push_back(pixels[index]);
                matrix.at(index_matrix).push_back(pixels[index + 1]);
                matrix.at(index_matrix).push_back(pixels[index + 2]);
                matrix.at(index_matrix).push_back(pixels[index + 3]);
            }
        }

        for (const auto& elm : matrix)
        {
            sf::Image des;
            des.create(constants::brick_width, constants::brick_height, elm.data());
            list.push_back(std::move(des));
        }

        initialized = true;
    }

    return list.at(static_cast<size_t>(property) - size_t(1));
}

sf::Texture& brick::get_texture(BrickProperty property)
{
    static sf::Texture empty;
    static sf::Texture brick;
    static sf::Texture diamond;
    static sf::Texture bomb;
    static sf::Texture scaleup;
    static sf::Texture clone;
    static bool initialized = false;
    static int retryCount = 0;
    while (!initialized)
    {
        try
        {
            if (!brick.loadFromImage(getImage(BRICK)))
            {
                throw std::logic_error("Get texture brick from image data has failed\n");
            }
            if (!diamond.loadFromImage(getImage(DIAMOND)))
            {
                throw std::logic_error("Get texture diamond from image data has failed\n");
            }
            if (!bomb.loadFromImage(getImage(BOMB)))
            {
                throw std::logic_error("Get texture bomb from image data has failed\n");
            }
            if (!scaleup.loadFromImage(getImage(SCALEUP)))
            {
                throw std::logic_error("Get texture scaleup from image data has failed\n");
            }
            if (!clone.loadFromImage(getImage(CLONE)))
            {
                throw std::logic_error("Get texture clone from image data has failed\n");
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Get texture failed: \n" << e.what() << "\n";
            if (retryCount < 2)
            {
                std::cout << "Retrying after 1 seconds...\n";
                retryCount++;
                sf::sleep(sf::seconds(1.0f));
                continue;
            }
            throw e;
        }

        initialized = true;
    }
    switch (property)
    {
    case BRICK:
        return brick;
    case DIAMOND:
        return diamond;
    case BOMB:
        return bomb;
    case SCALEUP:
        return scaleup;
    case CLONE:
        return clone;
    case NONE:
        [[fallthrough]];
    default:
        return empty;
    }
}

brick::brick(float px_x, float px_y, BrickProperty property) : m_wall(nullptr), m_property(property), m_hit_count(0)
{
    m_sprite.setTexture(get_texture(property));
    brick::init(px_x, px_y);
}

void brick::init(float px_x, float px_y)
{
    m_sprite.setPosition(px_x, px_y);
}

// Compute the brick's new position
void brick::update()
{}

void brick::registerHealthUpdate(const std::function<void(bool)>& fnc)
{
    if (m_property == brick::DIAMOND)
    {
        m_live_update_fnc = fnc;
        m_live_update_fnc(true);
    }
}

void brick::registerPontUpdate(const std::function<void(int16_t)>& fnc)
{
    if (m_property == brick::DIAMOND || m_property == brick::BRICK)
    {
        m_point_update_fnc = fnc;
    }
}

void brick::registerParent(wall* parent)
{
    m_wall = parent;
}

void brick::draw(sf::RenderWindow& window)
{
    // Ask the window to draw the sprite for us
    window.draw(m_sprite);
}

brick::BrickProperty brick::getProperty() const noexcept
{
    return m_property;
}

wall* brick::getWall() const noexcept
{
    return m_wall;
}

void brick::hit(HitInfo info) noexcept
{
    m_hit_count += info.damage;
    bool destroyed = false;
    switch (m_property)
    {
    case BRICK:
    {
        hit_brick(destroyed, info.damage_source);
    }
    break;
    case DIAMOND:
    {
        hit_diamond(destroyed, info.damage_source);
    }
    break;
    case BOMB:
    {
        hit_bomb(destroyed, info.damage_source);
    }
    break;
    case SCALEUP:
    {
        hit_scaleup(destroyed, info.damage_source);
    }
    case CLONE:
    {
        hit_clone(destroyed, info.damage_source);
    }
    break;
    case NONE:
        [[fallthrough]];
    default:
        break;
    }
    if (destroyed)
    {
        destroy();
    }
}

void brick::hit_clone(bool& destroyed, [[maybe_unused]] const HitSource hit_source)
{
    if (m_hit_count >= constants::cap_clone_hit)
    {
        destroyed = true;
    }
}

void brick::hit_scaleup(bool& destroyed, [[maybe_unused]] const HitSource hit_source)
{
    if (m_hit_count >= constants::cap_scaleup_hit)
    {
        destroyed = true;
    }
}

void brick::hit_bomb(bool& destroyed, const HitSource hit_source)
{
    if (m_hit_count >= constants::cap_bomb_hit)
    {
        destroyed = true;
    }
    if (hit_source == HitSource::FROM_BALL)
    {
        SoundManager::getInstance().playSoundEffect(SoundAdapter::get_sound_mode_id(SoundMode::BOMB_EXPLOSION));
    }
}

void brick::hit_diamond(bool& destroyed, const HitSource hit_source)
{
    if (m_hit_count >= constants::cap_diamond_hit)
    {
        destroyed = true;
        if (m_live_update_fnc)
        {
            m_live_update_fnc(false);
        }
        if (m_point_update_fnc)
        {
            m_point_update_fnc(int16_t(5));
        }
    }
    if (hit_source == HitSource::FROM_BALL)
    {
        SoundManager::getInstance().playSoundEffect(SoundAdapter::get_sound_mode_id(SoundMode::DIAMOND_DESTROY));
    }
}

void brick::hit_brick(bool& destroyed, HitSource hit_source)
{
    if (m_hit_count >= constants::cap_brick_hit)
    {
        destroyed = true;
        if (m_point_update_fnc)
        {
            m_point_update_fnc(int16_t(1));
        }
    }
    if (hit_source == HitSource::FROM_BALL)
    {
        SoundManager::getInstance().playSoundEffect(SoundAdapter::get_sound_mode_id(SoundMode::BRICK_BOUNCE));
    }
}

void wall::update_health(bool increase) noexcept
{
    increase ? ++live : --live;
}

void wall::bomb_exploding_at(const e_location& center, const float& radius) noexcept
{
    auto& a_brick = *this->m_data.at(center).get();
    wall* a_wall = this;

    auto px_x = center.x - floorf((static_cast<float>(radius) - 1.0F) / 2.0F) * a_brick.width();
    auto px_y = center.y - floorf((static_cast<float>(radius) - 1.0F) / 2.0F) * a_brick.height();

    for (int i = 0; i < radius; ++i)
    {
        for (int j = 0; j < radius; ++j)
        {
            const sf::Vector2f hit_point{px_x + static_cast<float>(i) * a_brick.width(),
                                         px_y + static_cast<float>(j) * a_brick.height()};

            auto& wall_data = a_wall->data();
            auto iter = wall_data.find(hit_point);
            if (iter != wall_data.end() && !iter->second->is_destroyed())
            {
                auto* alias = iter->second.get();
                alias->hit({constants::cap_brick_hit, brick::HitSource::FROM_EXPLOSION});
                if (alias->getProperty() == brick::BOMB)
                {
                    bomb_exploding_at(hit_point, radius);
                }
            }
        }
    }
}

void wall::reset_point() noexcept
{
    point = 0;
}

uint16_t wall::get_point() const noexcept
{
    return point;
}

void wall::update()
{
    for (auto& elm : this->m_data)
    {
        elm.second->update();
    }
}

void wall::draw(sf::RenderWindow& window)
{
    for (auto& elm : this->m_data)
    {
        elm.second->draw(window);
    }
}

void wall::init([[maybe_unused]] float, [[maybe_unused]] float)
{
    auto wall_data = constants::resouces_path + "wall.csv";

    if (!this->empty())
    {
        this->clear();
    }
    this->reset_point();

    const rapidcsv::Document doc(wall_data, rapidcsv::LabelParams(-1, -1));

    const auto padding = (constants::window_width - constants::brick_width * doc.GetColumnCount()) / 2;
    for (decltype(doc.GetRowCount()) i = 0; i < doc.GetRowCount(); i++)
    {
        const auto rows = doc.GetRow<int>(i);
        for (decltype(rows.size()) j = 0; j < rows.size(); j++)
        {
            const auto px_x = static_cast<float>(padding + j * constants::brick_width);
            const auto px_y = static_cast<float>(i * constants::brick_height);
            const auto property = static_cast<brick::BrickProperty>(doc.GetCell<int>(j, i));
            auto a_brick = std::make_unique<brick>(px_x, px_y, property);
            a_brick->registerHealthUpdate([this](bool is_increase) mutable { this->update_health(is_increase); });
            a_brick->registerPontUpdate([this](short point) mutable { this->update_point(point); });
            a_brick->registerParent(this);
            this->data().emplace(
                std::make_pair<const e_location, std::unique_ptr<brick>>({px_x, px_y}, std::move(a_brick)));
        }
    }
}

void wall::refresh()
{
    for (auto it = this->m_data.begin(); it != this->m_data.end();)
    {
        if (it->second->is_destroyed())
        {
            it = this->m_data.erase(it);
        }
        else
        {
            ++it;
        }
    }

    if (this->live <= 0)
    {
        this->destroy();
    }
}
