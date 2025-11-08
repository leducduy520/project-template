#ifndef BALL_H
#define BALL_H


#include "SFML/Audio.hpp"
#include "constants.hpp"
#include "entity.hpp"
#include <future>
#include <mutex>
#include <forward_list>

class ball : public moving_entity
{
    // Private data members
    static sf::Texture& get_texture();
    sf::Text m_text;
    std::atomic_bool m_running;
    std::atomic_bool m_pause;
    std::atomic_int m_strength;
    std::atomic<time_t> m_scale_time;
    std::future<void> m_result;
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
    void set_pause(const bool& pause) noexcept;
    void stop();
    std::pair<bool, time_t> get_scale_status() const noexcept;

    int get_strength() noexcept;

    std::forward_list<std::pair<sf::Vector2f, sf::Vector2f>> clone_balls;
    void clone();

private:
};

#endif // BALL_H
