#include "countingtext.hpp"
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <thread>

static std::string format_duration(time_t duration)
{
    // Calculate minutes and seconds
    int minutes = duration / 60;
    int seconds = duration % 60;

    // Create a stringstream to format the string
    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0') << minutes << ":" << std::setw(2) << std::setfill('0') << seconds;

    return ss.str();
}

CountingText::CountingText()
    : m_start_time(system_clock::now()), m_counting_time({}), m_limit({}), m_is_running(false), m_is_paused(false)
{}

void CountingText::start()
{
    m_start_time = system_clock::now();
    m_counting_time = {};
    m_is_paused = false;
    m_is_running = true;
}

void CountingText::restart()
{
    m_start_time = system_clock::now();
    m_counting_time = {};
}

void CountingText::stop()
{
    //std::cout << "Stop" << std::endl;
    m_is_running = false;
}

void CountingText::reset()
{
    m_start_time = {};
    m_counting_time = {};
    m_is_paused = false;
    m_is_running = false;
}

void CountingText::pause()
{
    m_is_paused = true;
}

void CountingText::stop_pause()
{
    m_is_paused = false;
}

void CountingText::setLimit(duration limit)
{
    m_limit = limit;
}

bool CountingText::is_timeout()
{
    auto left_time = duration_cast<seconds>(m_limit - m_counting_time).count();
    return left_time <= 0;
}

void CountingTextUpdate(CountingText* text)
{
    //std::cout << "Update" << std::endl;
    while (text->m_is_running)
    {
        if (!text->m_is_paused)
        {
            auto current_time = system_clock::now();
            text->m_counting_time = current_time - text->m_start_time;
            //std::cout << "Counting time: " << duration_cast<milliseconds>(text->m_counting_time).count() << " miliseconds" << std::endl;
            auto left_time = duration_cast<seconds>(text->m_limit - text->m_counting_time).count();
            text->setString(format_duration(left_time));
        }
        else
        {
            std::this_thread::sleep_for(100ms);
            text->m_start_time += 100ms;
        }
    }
}
