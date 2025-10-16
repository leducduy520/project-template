//#include "ThreadPoolGame.hpp"
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <thread>
#include "countingtext.hpp"
#include "ThreadPoolGame.hpp"
#include "helper.hpp"

void CountingTextUpdate(CountingText* text)
{
    //std::cout << "Update" << std::endl;
    while (text->m_is_running)
    {
        if (!text->m_is_paused)
        {
            auto current_time = system_clock::now();
            text->m_counting_time = current_time - text->m_start_time;
            auto left_time = duration_cast<seconds>(text->m_limit - text->m_counting_time).count();
            if (left_time < 0)
            {
                continue;
            }
            text->setString(utilities::texthelper::format_duration(left_time));
        }
        else
        {
            std::this_thread::sleep_for(100ms);
            text->m_start_time += 100ms;
        }
    }
}

CountingText::CountingText()
    : m_start_time(system_clock::now()), m_counting_time({}), m_limit({}), m_is_running(false), m_is_paused(false)
{}

CountingText::~CountingText()
{
    reset();
    if (m_result.valid())
    {
        m_result.get();
    }
}

void CountingText::start()
{
    m_start_time = system_clock::now();
    m_counting_time = {};
    m_is_paused = false;
    m_is_running = true;
    m_result = ThreadPool::getInstance().submit(1, CountingTextUpdate, this);
}

void CountingText::restart()
{
    m_start_time = system_clock::now();
    m_counting_time = {};
}

void CountingText::stop()
{
    m_is_paused = false;
    m_is_running = false;
    if (m_result.valid())
    {
        m_result.get();
    }
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

void CountingText::do_continue()
{
    m_is_paused = false;
}

void CountingText::set_limit(duration limit)
{
    m_limit = limit;
}

bool CountingText::is_timeout()
{
    auto left_time = duration_cast<seconds>(m_limit - m_counting_time).count();
    return (!m_is_running || left_time <= 0);
}
