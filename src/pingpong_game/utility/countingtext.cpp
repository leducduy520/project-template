//#include "ThreadPoolGame.hpp"
#include "countingtext.hpp"
#include "entitymanager.hpp"
#include "ball.hpp"
#include "ThreadPoolGame.hpp"
#include "helper.hpp"
#include "constants.hpp"
#include <spdlog/spdlog.h>
#include <thread>
#include <chrono>
#include <typeinfo>
#include <string>

using namespace std::chrono;
using namespace std::literals;

void CountingTextUpdate(CountingText* text)
{
    auto left_time = text->timer::leftTime().count();
    text->setString(utilities::texthelper::format_duration(left_time));
}

CountingText::CountingText() : Timer([this]() { CountingTextUpdate(this); })
{
    this->setOrigin({0, 0});
}

CountingText::~CountingText()
{
    stop();
}

void CountingText::start()
{
    timer::start();
}

void CountingText::restart()
{
    stop();
    set_limit(m_limit);
    start();
}

void CountingText::stop()
{
    timer::stop();
}

void CountingText::pause() noexcept
{
    timer::pause();
}

void CountingText::stop_pause() noexcept
{
    timer::stop_pause();
}

void CountingText::set_limit(duration limit)
{
    m_limit = limit;
    configure_timer(200, std::chrono::duration_cast<std::chrono::milliseconds>(m_limit).count());
}

void CountingText::add_time_and_restart(duration delta)
{
    auto left_time = timer::leftTime<std::milli>();
    auto new_limit = left_time + delta;
    configure_timer(200, std::chrono::duration_cast<std::chrono::milliseconds>(new_limit).count());
    timer::start();
}

bool CountingText::is_timeout() noexcept
{
    if (!timer::running()) {
        return true;
    }
    auto left_time = timer::leftTime<std::milli>().count();
    return left_time <= 0;
}

BallCountingText::~BallCountingText()
{
    m_associate = nullptr;
}

void BallCountingText::init(float px_x, float px_y)
{
    set_position(sf::Vector2f(px_x, px_y));
}

void BallCountingText::update()
{
    (void)0;
}

void BallCountingText::draw(sf::RenderWindow& window)
{
    window.draw(*this);
}

void BallCountingText::destroy() noexcept
{
    static_entity::destroy();
    try {
        CountingText::stop();
    }
    catch (...) {
        spdlog::error("Failed to stop CountingText in BallCountingText::destroy()");
    }
    m_associate = nullptr;
}

void BallCountingText::associate_with(Ientity* entity)
{
    using namespace utilities::texthelper;
    m_associate = dynamic_cast<ball*>(entity);
    if (m_associate != nullptr) {
        sf::Text::setPosition({m_associate->left(), m_associate->top() + m_associate->h()});
        CountingText::set_limit(seconds(10));
        CountingText::start();

        // Subscribe to ball destruction topic
        subscribe(Ientity::get_type_name<ball>() + "/destroyed");
        subscribe(Ientity::get_type_name<ball>() + "/update");
    }
}

void BallCountingText::on_message(const std::string& topic, Ientity* entity)
{
    if (m_associate == nullptr || entity->is_destroyed() || this->is_destroyed() ||
        entity->get_id() != m_associate->get_id())
        return;
    using namespace utilities::texthelper;
    const std::string ball_destruction_topic = Ientity::get_type_name<ball>() + "/destroyed";
    const std::string ball_update_topic = Ientity::get_type_name<ball>() + "/update";
    if (topic == ball_destruction_topic) {
        m_associate = nullptr;
        destroy();
        return;
    }
    if (topic == ball_update_topic) {
        if (is_timeout()) {
            m_associate->scale(1);
            destroy();
            return;
        }

        sf::Text::setPosition({m_associate->left(), m_associate->top() + m_associate->h() * 3 / 2});
    }
}
