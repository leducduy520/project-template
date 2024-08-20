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
    CountingText();
    void start();
    void restart();
    void stop();
    void reset();

    using time_point = system_clock::time_point;
    using duration = system_clock::duration;

    friend void CountingTextUpdate(CountingText* text);
private:
    time_point m_start_time;
    time_point m_end_time;
    duration m_counting_time;
    bool m_is_running;
    bool m_is_paused;
};

#endif // __ULTILITY_COUNTINGTEXT__
