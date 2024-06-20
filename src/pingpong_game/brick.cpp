#include "brick.hpp"
#include <algorithm>
#include "ball.hpp"
#include "interactions.hpp"

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

}

void wall::refresh(entity& e)
{
    auto b = dynamic_cast<ball*>(&e);
    if(b)
    {
        erase(std::remove_if(begin(), end(), [this, &b](brick& br){ handle_interaction(*b, br); return br.is_destroyed();}), end());
    }
    else{
        std::cerr << "The type of input entity must be ball\n";
    }
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
