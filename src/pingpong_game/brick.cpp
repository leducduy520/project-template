#include "brick.hpp"
#include "wallHelper.hpp"

// Define the static texture


sf::Texture &brick::getTexture(BrickProperty property)
{
    static sf::Texture empty;
    static sf::Texture brick;
    static sf::Texture diamond;
    static sf::Texture bomb;
    static bool initialized = false;
    if (!initialized) {
        if (!brick.loadFromFile(constants::resoucesPath + "brick.png")) {
            std::cerr << "Get texture brick failed\n";
        }
        if (!diamond.loadFromFile(constants::resoucesPath + "diamond.png")) {
            std::cerr << "Get texture diamond failed\n";
        }
        if (!bomb.loadFromFile(constants::resoucesPath + "bomb.png")) {
            std::cerr << "Get texture bomb failed\n";
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

void brick::releaseTexture()
{
    // sf::Texture empty;
    // std::swap(getTexture(), empty);
}

brick::brick(float x, float y, BrickProperty property)
    : m_property(property), m_hitCount(0)
{
    m_sprite.setTexture(getTexture(property));
    init(x, y);
}

void brick::init(float x, float y)
{
    m_sprite.setScale(constants::brick_width / m_sprite.getLocalBounds().width,
                    constants::brick_height / m_sprite.getLocalBounds().height);
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

void brick::hit(const int damage) noexcept
{
    m_hitCount += damage;
    bool destroyed = false;
    switch (m_property)
    {
    case BRICK:
        if(m_hitCount >= constants::cap_brick_hit)
            destroyed = true;
        break;
    case DIAMOND:
        if(m_hitCount >= constants::cap_diamond_hit)
            destroyed = true;    
        break;
    case BOMB:
        if(m_hitCount >= constants::cap_bomb_hit)
            destroyed = true;   
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
