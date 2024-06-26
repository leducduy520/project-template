#ifndef BALL_H
#define BALL_H



#include "constants.hpp"
#include "entity.hpp"
#include "SFML/Audio.hpp"

class ball : public moving_entity
{
    // Private data members
    static sf::Texture& getTexture();
    sf::Sound sound;
    sf::SoundBuffer buffer;

public:
    ball(float x, float y);
    ball();
    ~ball();

    void init(float x, float y) override;

    void update() override;
    void draw(sf::RenderWindow &window) override;

    void move_up() noexcept override;
    void move_down() noexcept override;
    void move_left() noexcept override;
    void move_right() noexcept override;
    void print_info() const noexcept override;

    enum SoundType
    {
        Edge,
        Brick
    };

    void playSound(SoundType type);
};

#endif // BALL_H
