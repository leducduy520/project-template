#include "helper.hpp"
#include "soundplayer.hpp"
#include <cstdlib>
#include <exception>
#include "brick.hpp"

extern template void wall::updatePoint<short>(short&& amount) noexcept;

using namespace std::literals;

sf::Image& getImage(brick::BrickProperty property)
{
    static bool initialized = false;
    static std::vector<sf::Image> list;
    if (!initialized)
    {
        sf::Image source;
        if (!source.loadFromFile(constants::resoucesPath + "brick.png"))
        {
            const std::string message = "Cannot open source image "s + constants::resoucesPath + "brick.png";
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

sf::Texture& brick::getTexture(BrickProperty property)
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

brick::brick(float px_x, float px_y, BrickProperty property) : m_wall(nullptr), m_property(property), m_hitCount(0)
{
    m_sprite.setTexture(getTexture(property));
    brick::init(px_x, px_y);
}

void brick::init(float px_x, float px_y)
{
    m_sprite.setPosition(px_x, px_y);
}

// Compute the brick's new position
void brick::update()
{}

void brick::registerLiveUpdate(const std::function<void(bool)>& fnc)
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

void brick::hit(const int damage, const bool relate) noexcept
{
    m_hitCount += damage;
    bool destroyed = false;
    switch (m_property)
    {
    case BRICK:
    {
        hit_brick(destroyed, relate);
    }
    break;
    case DIAMOND:
    {
        hit_diamond(destroyed, relate);
    }
    break;
    case BOMB:
    {
        hit_bomb(destroyed, relate);
    }
    break;
    case SCALEUP:
    {
        hit_scaleup(destroyed, relate);
    }
    case CLONE:
    {
        hit_clone(destroyed, relate);
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

void brick::hit_clone(bool& destroyed, const bool relate)
{
    if (m_hitCount >= constants::cap_clone_hit)
    {
        destroyed = true;
    }
    if (!relate)
    {
        // SoundPlayer::getInstance()->playSound(SoundPlayer::SCALEUP_EFFECT);
    }
}

void brick::hit_scaleup(bool& destroyed, const bool relate)
{
    if (m_hitCount >= constants::cap_scaleup_hit)
    {
        destroyed = true;
    }
    if (!relate)
    {
        // SoundPlayer::getInstance()->playSound(SoundPlayer::SCALEUP_EFFECT);
    }
}

void brick::hit_bomb(bool& destroyed, const bool relate)
{
    if (m_hitCount >= constants::cap_bomb_hit)
    {
        destroyed = true;
    }
    if (!relate)
    {
        SoundPlayer::getInstance()->playSound(SoundPlayer::BOMB_EXPLOSION);
    }
}

void brick::hit_diamond(bool& destroyed, const bool relate)
{
    if (m_hitCount >= constants::cap_diamond_hit)
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
    if (!relate)
    {
        SoundPlayer::getInstance()->playSound(SoundPlayer::DIAMOND_DESTROY);
    }
}

void brick::hit_brick(bool& destroyed, const bool relate)
{
    if (m_hitCount >= constants::cap_brick_hit)
    {
        destroyed = true;
        if (m_point_update_fnc)
        {
            m_point_update_fnc(int16_t(1));
        }
    }
    if (!relate)
    {
        SoundPlayer::getInstance()->playSound(SoundPlayer::BRICK_BOUNCE);
    }
}

void wall::updateLive(bool increase) noexcept
{
    increase ? ++live : --live;
}

void wall::resetPoint() noexcept
{
    point = 0;
}

uint16_t wall::getPoint() const noexcept
{
    return point;
}

void wall::update()
{
    for (auto& elm : *this)
    {
        elm.second->update();
    }
}

void wall::draw(sf::RenderWindow& window)
{
    for (auto& elm : *this)
    {
        elm.second->draw(window);
    }
}

void wall::init([[maybe_unused]] float px_x, [[maybe_unused]] float px_y)
{
    utilities::wallhelper::buildWall(*this, (constants::resoucesPath + "wall.csv").c_str());
}

void wall::refresh()
{
    for (auto it = this->begin(); it != this->end();)
    {
        if (it->second->is_destroyed())
        {
            it = this->erase(it);
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
