#ifndef BALL_H
#define BALL_H


#include "SFML/Audio.hpp"
#include "constants.hpp"
#include "entity.hpp"
#include <future>
#include <mutex>

class ball : public moving_entity
{
    // Private data members
    static sf::Texture& getTexture();
    std::atomic_bool m_scaleflag;
    std::atomic_bool m_pauseflag;
    std::atomic_int m_strength;
    std::atomic<time_t> m_scale_time;
    std::mutex m_mt;

public:
    ball(float px_x, float px_y);
    ball();
    ~ball();

    void init(float px_x, float px_y) override;

    void update() override;
    void draw(sf::RenderWindow& window) override;

    void move_up() noexcept override;
    void move_down() noexcept override;
    void move_left() noexcept override;
    void move_right() noexcept override;
    void print_info() const noexcept override;

    void scale(const int& n) noexcept;
    void reset_size() noexcept;
    void set_pause(const bool& pause);
    void stop();
    int get_strength() noexcept;

private:
};

#endif // BALL_H
