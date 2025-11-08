//#include "ThreadPoolGame.hpp"
#include "countingtext.hpp"
#include "ThreadPoolGame.hpp"
#include "helper.hpp"
#include "constants.hpp"
#include <thread>
#include <chrono>

using namespace std::chrono;
using namespace std::literals;

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
                std::this_thread::sleep_for(100ms); // Prevent busy-wait
                continue;
            }
            text->setString(utilities::texthelper::format_duration(left_time));
            std::this_thread::sleep_for(100ms); // Prevent busy-wait loop
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
        m_result.wait();
    }
}

void CountingText::start()
{
    m_start_time = std::chrono::system_clock::now();
    m_counting_time = {};
    m_is_paused = false;
    m_is_running = true;
    m_result = ThreadPool::getInstance().submit(1, CountingTextUpdate, this);
}

void CountingText::restart() noexcept
{
    m_start_time = std::chrono::system_clock::now();
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

void CountingText::reset() noexcept
{
    m_start_time = {};
    m_counting_time = {};
    m_is_paused = false;
    m_is_running = false;
}

void CountingText::pause() noexcept
{
    m_is_paused = true;
}

void CountingText::stop_pause() noexcept
{
    m_is_paused = false;
}

void CountingText::set_limit(duration limit) noexcept
{
    m_limit = limit;
}

bool CountingText::is_timeout() noexcept
{
    auto left_time = std::chrono::duration_cast<std::chrono::seconds>(m_limit - m_counting_time).count();
    return (!m_is_running || left_time <= 0);
}
