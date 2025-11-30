#ifndef __ULTILITY_COUNTINGTEXT__
#define __ULTILITY_COUNTINGTEXT__

#include "SFML/Graphics.hpp"
#include "ball.hpp"
#include "timer.hpp"
#include <chrono>
#include <future>
#include <functional>

class CountingText;
class entity_manager;

void CountingTextUpdate(CountingText* text);

class CountingText : public sf::Text, private Timer<std::milli>
{
public:
    using sclock = std::chrono::steady_clock;
    using duration = sclock::duration;
    using time_point = sclock::time_point;
    using timer = Timer<std::milli>;

    CountingText();
    ~CountingText();

    void start();
    void restart();
    void stop();
    void pause() noexcept;
    void stop_pause() noexcept;

    bool is_timeout() noexcept;

    void set_limit(duration limit);
    void add_time_and_restart(duration delta);


    friend void CountingTextUpdate(CountingText* text);

private:
    void configure_timer(size_t interval, size_t total)
    {
        timer::configure<std::milli>(std::chrono::milliseconds(interval), total / interval);
    }

    duration m_limit = duration::zero();
};

class BallCountingText : public CountingText, public Ientity
{
public:
    BallCountingText() = default;
    BallCountingText(const BallCountingText& other) = delete;
    BallCountingText& operator=(const BallCountingText& other) = delete;

    BallCountingText(BallCountingText&& other) noexcept
    {
        if (this == &other) {
            return;
        }
        m_associate = other.m_associate;
        other.m_associate = nullptr;
    }

    BallCountingText& operator=(BallCountingText&& other) noexcept
    {
        if (this == &other) {
            return *this;
        }
        m_associate = other.m_associate;
        other.m_associate = nullptr;
        return *this;
    }

    ~BallCountingText();

    void init(float px_x, float px_y) override;
    void update() override;
    void draw(sf::RenderWindow& window) override;
    void destroy() noexcept override;

    void associate_with(Ientity* entity);

    using sf::Drawable::draw;

protected:
    // Override on_message to handle ball destruction events
    void on_message(const std::string& topic, Ientity* entity) override;

private:
    ball* m_associate = nullptr;
};

#endif // __ULTILITY_COUNTINGTEXT__
