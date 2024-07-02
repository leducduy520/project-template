#include "brick.hpp"
#include "wallHelper.hpp"
#include <exception>
#include "stdlib.h"
#include "soundplayer.hpp"

// Define the static texture

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
            std::string message = "Cannot open source image "s + constants::resoucesPath + "brick.png";
            throw std::logic_error(message.c_str());
        }
        
        const auto width = source.getSize().x;
        const auto height = source.getSize().y;
        const auto pixels = source.getPixelsPtr();
        std::vector<std::vector<sf::Uint8>> matrix(width / constants::brick_width);
        
        for (auto y = 0; y < height; ++y)
        {
            for (auto x = 0; x < width; ++x)
            {
                auto index = (x + y * width) * 4; // 4 bytes per pixel (RGBA)
                auto index_matrix = x / constants::brick_width;
                matrix.at(index_matrix).push_back(pixels[index]);
                matrix.at(index_matrix).push_back(pixels[index + 1]);
                matrix.at(index_matrix).push_back(pixels[index + 2]);
                matrix.at(index_matrix).push_back(pixels[index + 3]);
            }
        }

        for (int i = 0; i < matrix.size(); i++)
        {
            sf::Image des;
            des.create(constants::brick_width, constants::brick_height, &matrix[i][0]);
            list.push_back(std::move(des));
        }

        initialized = true;
    }
    
    return list.at(static_cast<int>(property) - 1);
}

sf::Texture &brick::getTexture(BrickProperty property)
{
    retry:
    static sf::Texture empty;
    static sf::Texture brick;
    static sf::Texture diamond;
    static sf::Texture bomb;
    static bool initialized = false;
    static int retryCount = 0;
    if (!initialized) {
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
        catch (const std::exception& e)
        {
            std::cerr << "Get texture failed: \n" << e.what() << "\n";
            if (retryCount < 5)
            {
                std::cout << "Retrying after 5 seconds...\n";
                retryCount++;
                sf::sleep(sf::seconds(5.0f));
                goto retry;
            }
            throw e;
        }
        
        initialized = true;
    }
    switch (property)
    {
    case BRICK:
        return brick;
        break;
    case DIAMOND:
        return diamond;
        break;
    case BOMB:
        return bomb;
        break;
    default:
        return empty;
        break;
    }
}

brick::brick(float x, float y, BrickProperty property)
    : m_property(property), m_hitCount(0)
{
    m_sprite.setTexture(getTexture(property));
    init(x, y);
}

void brick::init(float x, float y)
{
    m_sprite.setPosition(x, y);
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
            destroyed = true;
        if(!relate)
            SoundPlayer::getInstance()->playSound(SoundPlayer::BRICK_BOUNCE);
    }
        break;
    case DIAMOND:
    {
        if (m_hitCount >= constants::cap_diamond_hit)
            destroyed = true;
        if(!relate)
            SoundPlayer::getInstance()->playSound(SoundPlayer::DIAMOND_DESTROY);
    }
        break;
    case BOMB:
    {
        if (m_hitCount >= constants::cap_bomb_hit)
            destroyed = true;
        if(!relate)
            SoundPlayer::getInstance()->playSound(SoundPlayer::BOMB_EXPLOSION);
    }
        break;
    default:
        break;
    }
    if (destroyed)
        destroy();
}

void wall::update()
{
    for(auto it = this->begin(); it != this->end();++it)
    {
        it->second->update();
    }
}

void wall::draw(sf::RenderWindow &window)
{
    for(auto it = this->begin(); it != this->end();++it)
    {
        it->second->draw(window);
    }
}

void wall::init(float x, float y)
{
    wall_utils::createWall(*this, (constants::resoucesPath + "wall.csv").c_str());
}
