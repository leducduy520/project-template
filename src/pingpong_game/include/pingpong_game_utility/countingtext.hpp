#ifndef __ULTILITY_COUNTINGTEXT__
#define __ULTILITY_COUNTINGTEXT__

#include "SFML/Graphics.hpp"
#include <chrono>
#include <future>

class CountingText;

void CountingTextUpdate(CountingText* text);

class CountingText : public sf::Text
{
public:
    using duration = std::chrono::system_clock::duration;
    using time_point = std::chrono::system_clock::time_point;

    CountingText();
    ~CountingText();
    void start();
    void restart() noexcept;
    void stop();
    void reset() noexcept;
    void pause() noexcept;
    void stop_pause() noexcept;
    void set_limit(duration limit) noexcept;
    bool is_timeout() noexcept;


    friend void CountingTextUpdate(CountingText* text);

private:
    std::future<void> m_result;
    time_point m_start_time;
    duration m_counting_time;
    duration m_limit;
    bool m_is_running;
    bool m_is_paused;
};

#endif // __ULTILITY_COUNTINGTEXT__
