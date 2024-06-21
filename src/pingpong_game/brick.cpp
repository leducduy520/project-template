#include "brick.hpp"
#include <algorithm>
#include "ball.hpp"
#include "interactions.hpp"
#include "wallHelper.hpp"

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
    sprite.setScale(constants::brick_width / sprite.getLocalBounds().width,
                    constants::brick_height / sprite.getLocalBounds().height);
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

void wall::update()
{
    for(auto it = this->begin(); it != this->end();++it)
    {
        it->update();
    }
}

void wall::draw(sf::RenderWindow &window)
{
    for(auto it = this->begin(); it != this->end();++it)
    {
        it->draw(window);
    }
}

void wall::init(float x, float y)
{
    wall_utils::createWall(*this, (constants::resoucesPath + "wall.csv").c_str());
}
