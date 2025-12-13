#include "LoginGame.hpp"
#include "constants.hpp"
#include "helper.hpp"
#include <chrono>
#include <memory>
#include <thread>

namespace
{
    constexpr unsigned int INPUT_BOUND_WIDTH = 200;
    constexpr unsigned int INPUT_BOUND_HEIGHT = 50;
    constexpr unsigned int FONT_SIZE = 28;
    constexpr unsigned int WINDOW_WIDTH = 800;
    constexpr unsigned int WINDOW_HEIGHT = 600;
    constexpr unsigned int TEXT_LIMIT = 16;
} // namespace

using namespace utilities;
using namespace std::chrono_literals;

void LoginWindow::login(const std::string& username, const std::string& password)
{
    if (!username.empty() && !password.empty()) {
        m_loginSuccess = true;
        return;
    }
}

LoginWindow::LoginWindow() : m_focusedName(true), m_loginSuccess(false), m_blink_run(true)
{
    using namespace utilities;

    m_window.create(sf::VideoMode(sf::Vector2u{WINDOW_WIDTH, WINDOW_HEIGHT}), "Login Window");
    m_window.setPosition(sf::Vector2i{(1920 - WINDOW_WIDTH) / 2, (1080 - WINDOW_HEIGHT) / 2});
    m_window.setFramerateLimit(120);

    m_textname = std::make_unique<sf::Text>(constants::null_font);
    m_textname->setFillColor(sf::Color{128, 128, 128, 128});
    m_textname->setCharacterSize(FONT_SIZE);
    m_textname->setFont(texthelper::getFont(texthelper::MODESTICSANS_BOLDITALIC));

    m_textpass = std::make_unique<sf::Text>(constants::null_font);
    m_textpass->setFillColor(sf::Color{128, 128, 128, 128});
    m_textpass->setCharacterSize(FONT_SIZE);
    m_textpass->setFont(texthelper::getFont(texthelper::MODESTICSANS_BOLDITALIC));

    m_static_name = std::make_unique<sf::Text>(constants::null_font);
    m_static_name->setString("Name: ");
    m_static_name->setFillColor(sf::Color::Black);
    m_static_name->setCharacterSize(FONT_SIZE);
    m_static_name->setFont(texthelper::getFont(texthelper::MODESTICSANS_BOLDITALIC));
    texthelper::aligning::Aligning(m_static_name.get(), sf::FloatRect{{200, 235}, {200, 50}}, texthelper::aligning::ML);

    m_static_pass = std::make_unique<sf::Text>(constants::null_font);
    m_static_pass->setString("Password: ");
    m_static_pass->setFillColor(sf::Color::Black);
    m_static_pass->setCharacterSize(FONT_SIZE);
    m_static_pass->setFont(texthelper::getFont(texthelper::MODESTICSANS_BOLDITALIC));
    texthelper::aligning::Aligning(m_static_pass.get(), sf::FloatRect{{200, 315}, {200, 50}}, texthelper::aligning::ML);

    m_blink_fut = std::async(std::launch::async, &LoginWindow::blinkAnimation, this);
}

std::pair<bool, std::string> LoginWindow::run()
{
    while (m_window.isOpen()) {
        listening();
        update();
        render();
    }
    return std::make_pair(m_loginSuccess, m_textname->getString());
}

void LoginWindow::listening()
{
    while (const auto event = m_window.pollEvent()) {
        if (!event.has_value()) {
            return;
        }
        const auto event_value = event.value();
        if (const auto ptr = event_value.getIf<sf::Event::TextEntered>(); ptr) {
            
            auto code = ptr->unicode;
            if (code < 128 && code != 0x09) {
                updateText(code);
            }
        }
        if (event->is<sf::Event::KeyPressed>()) {
            handleKeyPress(event.value());
        }
    }
}

void LoginWindow::update()
{
    texthelper::aligning::Aligning(m_textname.get(), sf::FloatRect{{400, 235}, {200, 50}}, texthelper::aligning::MR);
    texthelper::aligning::Aligning(m_textpass.get(), sf::FloatRect{{400, 315}, {200, 50}}, texthelper::aligning::MR);
}

void LoginWindow::render()
{
    if (m_window.isOpen()) {
        m_window.clear(sf::Color{223, 228, 210, 255});
        m_window.draw(*m_textname);
        m_window.draw(*m_static_name);
        m_window.draw(*m_textpass);
        m_window.draw(*m_static_pass);
        m_window.display();
    }
}

void LoginWindow::updateText(char32_t code)
{
    sf::String str = m_focusedName.load() ? m_textname->getString() : m_textpass->getString();

    if (!str.isEmpty() && str.getSize() <= TEXT_LIMIT && (code == 0x7F || code == 0x08)) {
        str.erase(str.getSize() - 1);
        if (m_focusedName.load()) {
            m_textname->setString(str);
        }
        else {
            m_textpass->setString(str);
            m_strpass.erase(m_strpass.length() - 1);
        }
    }
    else if (m_focusedName.load() && code > 20) {
        str += static_cast<char>(code);
        m_textname->setString(str);
    }
    else if (code > 20) {
        str += '*';
        m_strpass += static_cast<char>(code);
        m_textpass->setString(str);
    }
}

void LoginWindow::handleKeyPress(const sf::Event& event)
{
    switch (event.getIf<sf::Event::KeyPressed>()->code) {
    case sf::Keyboard::Key::Tab:
    {
        m_focusedName.store(!m_focusedName.load());
        if (m_focusedName.load()) {
            m_static_name->setCharacterSize(FONT_SIZE + 5);
            m_static_pass->setCharacterSize(FONT_SIZE);
        }
        else {
            m_static_pass->setCharacterSize(FONT_SIZE + 5);
            m_static_name->setCharacterSize(FONT_SIZE);
        }
    } break;
    case sf::Keyboard::Key::Enter:
    {
        if (!m_textname->getString().isEmpty() && !m_strpass.empty()) {
            login(m_textname->getString(), m_strpass);
            if (m_loginSuccess) {
                m_window.close();
                m_blink_run = false;
                m_blink_fut.wait();
            }
        }
    } break;
    case sf::Keyboard::Key::Escape:
    {
        m_window.close();
        m_blink_run = false;
        m_blink_fut.wait();
    } break;
    default:
        break;
    }
}

void LoginWindow::blinkAnimation()
{
    std::unique_lock<std::mutex> lock(m_blink_mt, std::defer_lock);
    while (m_blink_run) {
        std::this_thread::sleep_for(500ms);

        lock.lock();
        m_focusedName.load() ? m_static_name->setCharacterSize(FONT_SIZE) : m_static_pass->setCharacterSize(FONT_SIZE);
        std::this_thread::sleep_for(500ms);
        m_focusedName.load() ? m_static_name->setCharacterSize(FONT_SIZE + 5)
                             : m_static_pass->setCharacterSize(FONT_SIZE + 5);
        lock.unlock();
    }
}
