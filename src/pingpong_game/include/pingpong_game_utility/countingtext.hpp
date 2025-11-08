#ifndef __ULTILITY_COUNTINGTEXT__
#define __ULTILITY_COUNTINGTEXT__

#include "SFML/Graphics.hpp"
#include <chrono>
#include <future>

using namespace std;
using namespace std::chrono;
using namespace std::literals;

class CountingText;

void CountingTextUpdate(CountingText* text);

class CountingText : public sf::Text
{
public:
    using duration = system_clock::duration;
    using time_point = system_clock::time_point;

    CountingText();
    ~CountingText();
    void start();
    void restart();
    void stop();
    void reset();
    void pause();
    void stop_pause();
    void set_limit(duration limit);
    bool is_timeout();


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
