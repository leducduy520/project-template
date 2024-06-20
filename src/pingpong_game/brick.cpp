#include "brick.hpp"

// Define the static texture


sf::Texture &brick::getTexture()
{
    static sf::Texture texture;
    static bool initialized = false;
    if (!initialized) {
        if (!texture.loadFromFile(constants::resoucesPath + "brick.png")) {
            std::cerr << "Get texture failed\n";
        }
        initialized = true;
    }
    return texture;
}

void brick::releaseTexture()
{
    sf::Texture empty;
    std::swap(getTexture(), empty); 
}

brick::brick(float x, float y) : entity()
{
    init(x, y);
}

void brick::init(float x, float y)
{
    sprite.setTexture(getTexture());
    sprite.setScale(constants::brick_width / get_bounding_box().width,
                    constants::brick_height / get_bounding_box().height);
    sprite.setPosition(x, y);
}

// Compute the brick's new position
void brick::update()
{
}

void brick::draw(sf::RenderWindow &window)
{
    // Ask the window to draw the sprite for us
    window.draw(sprite);
}

void wall::release()
{
    while (size())
    {
        pop_front();
        if(size() == 1)
        {
            front().releaseTexture();
        }
    }
    clear();
}

wall::~wall()
{
    release();
}
