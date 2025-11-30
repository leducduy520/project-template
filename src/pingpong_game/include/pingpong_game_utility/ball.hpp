#ifndef BALL_H
#define BALL_H


#include "SFML/Audio.hpp"
#include "SFML/System/Thread.hpp"
#include "constants.hpp"
#include "entity.hpp"
#include <future>
#include <mutex>
#include <forward_list>

class ball : public moving_entity
{
    // Private data members
    static sf::Texture& get_texture();
    std::atomic_int m_strength;
    size_t m_counting_text_id{0};

public:
    ball(float px_x, float px_y);
    ball();
    ~ball() = default;

    void init(float px_x, float px_y) override;
    void update() override;
    void draw(sf::RenderWindow& window) override;

    void move_up() noexcept override;
    void move_down() noexcept override;
    void move_left() noexcept override;
    void move_right() noexcept override;
    void scale(const int& n) noexcept;
    void destroy() noexcept override;

    void set_counting_text_id(size_t id) noexcept;

    int strength() noexcept;
    size_t get_counting_text_id() const noexcept;
};

#endif // BALL_H
