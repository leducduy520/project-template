#include "brick.hpp"
#include "wallHelper.hpp"

// Define the static texture

sf::Image& getImage(brick::BrickProperty property)
{
    static bool initialized = false;
    static std::vector<sf::Image> list;
    if (!initialized)
    {
        sf::Image source;
        if (!source.loadFromFile(constants::resoucesPath + "brick.png"))
        {
            throw std::exception("Cannot open source image");
        }
        
        const auto width = source.getSize().x;
        const auto height = source.getSize().y;
        const auto pixels = source.getPixelsPtr();
        std::vector<std::vector<sf::Uint8>> matrix(width / constants::brick_width);
        
        for (unsigned int y = 0; y < height; ++y)
        {
            for (unsigned int x = 0; x < width; ++x)
            {
                unsigned int index = (x + y * width) * 4; // 4 bytes per pixel (RGBA)
                for (int k = 0; k < matrix.size(); k++)
                {
                    if (k == (x / constants::brick_width))
                    {
                        matrix.at(k).push_back(pixels[index]);
                        matrix.at(k).push_back(pixels[index + 1]);
                        matrix.at(k).push_back(pixels[index + 2]);
                        matrix.at(k).push_back(pixels[index + 3]);
                    }
                }
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
    static sf::Texture empty;
    static sf::Texture brick;
    static sf::Texture diamond;
    static sf::Texture bomb;
    static bool initialized = false;
    if (!initialized) {
        getImage(BRICK);
        if (!brick.loadFromImage(getImage(BRICK)))
        {
            std::cerr << "Get texture brick failed\n";
        }
        if (!diamond.loadFromImage(getImage(DIAMOND)))
        {
            std::cerr << "Get texture diamond failed\n";
        }
        if (!bomb.loadFromImage(getImage(BOMB)))
        {
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
