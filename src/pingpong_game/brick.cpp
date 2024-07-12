#include "brick.hpp"
#include "soundplayer.hpp"
#include "wallHelper.hpp"
#include <cstdlib>
#include <exception>

// Define the static texture

using namespace std::literals;

sf::Image &getImage(brick::BrickProperty property)
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
        const auto *const pixels = source.getPixelsPtr();
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

        for (const auto &elm : matrix)
        {
            sf::Image des;
            des.create(constants::brick_width, constants::brick_height, elm.data());
            list.push_back(std::move(des));
        }

        initialized = true;
    }

    return list.at(static_cast<size_t>(property) - size_t(1));
}

sf::Texture &brick::getTexture(BrickProperty property)
{
    static sf::Texture empty;
    static sf::Texture brick;
    static sf::Texture diamond;
    static sf::Texture bomb;
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
        }
        catch (const std::exception &e)
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
    case NONE:
        [[fallthrough]];
    default:
        return empty;
    }
}

brick::brick(float px_x, float px_y, BrickProperty property) : m_property(property), m_hitCount(0)
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
{
}

void brick::draw(sf::RenderWindow &window)
{
    // Ask the window to draw the sprite for us
    window.draw(m_sprite);
}

brick::BrickProperty brick::getProperty() const noexcept
{
    return m_property;
}

void brick::hit(const int damage, const bool relate) noexcept
{
    m_hitCount += damage;
    bool destroyed = false;
    switch (m_property)
    {
    case BRICK:
    {
        if (m_hitCount >= constants::cap_brick_hit)
        {
            destroyed = true;
        }
        if (!relate)
        {
            SoundPlayer::getInstance()->playSound(SoundPlayer::BRICK_BOUNCE);
        }
    }
    break;
    case DIAMOND:
    {
        if (m_hitCount >= constants::cap_diamond_hit)
        {
            destroyed = true;
        }
        if (!relate)
        {
            SoundPlayer::getInstance()->playSound(SoundPlayer::DIAMOND_DESTROY);
        }
    }
    break;
    case BOMB:
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

void wall::update()
{
    for (auto it = this->begin(); it != this->end(); ++it)
    {
        it->second->update();
    }
}

void wall::draw(sf::RenderWindow &window)
{
    for (auto it = this->begin(); it != this->end(); ++it)
    {
        it->second->draw(window);
    }
}

void wall::init([[maybe_unused]] float px_x, [[maybe_unused]] float px_y)
{
    wall_utils::createWall(*this, (constants::resoucesPath + "wall.csv").c_str());
}
