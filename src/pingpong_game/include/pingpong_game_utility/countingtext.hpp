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

class BallCountingText : public CountingText, public static_entity
{
public:
    BallCountingText() = default;
    BallCountingText(const BallCountingText& other) = delete;
    BallCountingText& operator=(const BallCountingText& other) = delete;

    BallCountingText(BallCountingText&& other) noexcept : CountingText(), static_entity(std::move(other))
    {
        if (this == &other) {
            return;
        }
        // Move CountingText base class (sf::Text part)
        // Note: Timer<std::milli> is private base, so we rely on CountingText's default construction
        // and move the sf::Text part which contains the visual state
        static_cast<sf::Text&>(*this) = std::move(static_cast<sf::Text&>(other));
        // Note: m_limit and Timer state cannot be moved directly due to private access
        // The timer will need to be restarted if needed after move
        // Move member
        m_associate = other.m_associate;
        other.m_associate = nullptr;
    }

    BallCountingText& operator=(BallCountingText&& other) noexcept
    {
        if (this == &other) {
            return *this;
        }
        // Move assign CountingText base class (sf::Text part)
        static_cast<sf::Text&>(*this) = std::move(static_cast<sf::Text&>(other));
        // Note: m_limit and Timer state cannot be moved directly due to private access
        // The timer will need to be restarted if needed after move
        // Move assign static_entity base class
        static_cast<static_entity&>(*this) = std::move(static_cast<static_entity&>(other));
        // Move member
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
