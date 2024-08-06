#include "LoginGame.hpp"
#include "DBClientGame.hpp"
#include "constants.hpp"
#include "helper.hpp"
#include <memory>

#define INPUT_BOUND_WIDTH 200
#define INPUT_BOUND_HEIGHT 50
#define FONT_SIZE 28
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define TEXT_LIMIT 16

using namespace utilities;

void LoginWindow::login(const std::string& username, const std::string& password)
{
    try
    {
        DBINSTANCE->GetDatabase("duyld");
        if (!DBINSTANCE->GetCollection("pingpong_game"))
        {
            DBINSTANCE->CreateCollection("pingpong_game");
            if (DBINSTANCE->InsertDocument(make_document(kvp("name", username), kvp("password", password))))
            {
                cout << "successfully insert initial data for " << username << '\n';
                m_loginSuccess = true;
                return;
            }
            throw std::runtime_error("Failed to insert initial data for "s + username);
        }
        if (DBINSTANCE->GetDocument(make_document(kvp("name", username))))
        {
            cout << "User " << username << " exists" << '\n';
            if (DBINSTANCE->GetDocument(make_document(kvp("name", username), kvp("password", password))))
            {
                cout << "User " << username << " login successfully\n";
                m_loginSuccess = true;
                return;
            }
            throw std::runtime_error("It's not correct password for user "s + username);
        }
        if (DBINSTANCE->InsertDocument(make_document(kvp("name", username), kvp("password", password))))
        {
            cout << "successfully insert initial data for " << username << '\n';
            m_loginSuccess = true;
            return;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

LoginWindow::LoginWindow() : m_focusedName(true), m_loginSuccess(false), m_blink_run(false)
{
    m_window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Login Window");
    m_window.setPosition(sf::Vector2i{(1920 - WINDOW_WIDTH) / 2, (1080 - WINDOW_HEIGHT) / 2});
    m_window.setFramerateLimit(120);

    m_font.loadFromFile(constants::resoucesPath + "ModesticSans/ModesticSans-BoldItalic.ttf");

    m_textname = make_unique<sf::Text>();
    m_textname->setFillColor(sf::Color{128, 128, 128, 128});
    m_textname->setCharacterSize(FONT_SIZE);
    m_textname->setFont(m_font);

    m_textpass = make_unique<sf::Text>();
    m_textpass->setFillColor(sf::Color{128, 128, 128, 128});
    m_textpass->setCharacterSize(FONT_SIZE);
    m_textpass->setFont(m_font);

    m_static_name = make_unique<sf::Text>();
    m_static_name->setString("Name: ");
    m_static_name->setFillColor(sf::Color::Black);
    m_static_name->setCharacterSize(FONT_SIZE);
    m_static_name->setFont(m_font);
    texthelper::aligning::Aligning(m_static_name.get(), sf::FloatRect{{200, 235}, {200, 50}}, texthelper::aligning::ML);

    m_static_pass = make_unique<sf::Text>();
    m_static_pass->setString("Password: ");
    m_static_pass->setFillColor(sf::Color::Black);
    m_static_pass->setCharacterSize(FONT_SIZE);
    m_static_pass->setFont(m_font);
    texthelper::aligning::Aligning(m_static_pass.get(), sf::FloatRect{{200, 315}, {200, 50}}, texthelper::aligning::ML);

    m_blink_run = true;
    m_blink_fut = std::async(std::launch::async, &LoginWindow::blinkAnimation, this);
}

std::pair<bool, std::string> LoginWindow::run()
{
    while (m_window.isOpen())
    {
        listening();
        update();
        render();
    }
    return std::make_pair(m_loginSuccess, m_textname->getString());
}

void LoginWindow::listening()
{
    sf::Event event{};
    while (m_window.pollEvent(event))
    {
        if (event.type == sf::Event::TextEntered)
        {
            auto code = event.text.unicode;
            if (code < 128 && code != 0x09)
            {
                updateText(code);
            }
        }
        if (event.type == sf::Event::KeyPressed)
        {
            handleKeyPress(event);
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
    if (m_window.isOpen())
    {
        m_window.clear(sf::Color{223, 228, 210, 255});
        m_window.draw(*m_textname);
        m_window.draw(*m_static_name);
        m_window.draw(*m_textpass);
        m_window.draw(*m_static_pass);
        m_window.display();
    }
}

void LoginWindow::updateText(const sf::Uint32& code)
{
    sf::String str = m_focusedName.load() ? m_textname->getString() : m_textpass->getString();

    if (str.getSize() > 0 && str.getSize() <= TEXT_LIMIT && (code == 0x7F || code == 0x08))
    {
        str.erase(str.getSize() - 1);
        if (m_focusedName.load())
        {
            m_textname->setString(str);
        }
        else
        {
            m_textpass->setString(str);
            m_strpass.erase(m_strpass.length() - 1);
        }
    }
    else if (m_focusedName.load() && code > 20)
    {
        str += static_cast<char>(code);
        //m_strname = str;
        m_textname->setString(str);
    }
    else if (code > 20)
    {
        str += '*';
        m_strpass += static_cast<char>(code);
        m_textpass->setString(str);
    }
}

void LoginWindow::handleKeyPress(const sf::Event& event)
{
    switch (event.key.code)
    {
    case sf::Keyboard::Tab:
    {
        m_focusedName.store(!m_focusedName.load());
        if (m_focusedName.load())
        {
            m_static_name->setCharacterSize(FONT_SIZE + 5);
            m_static_pass->setCharacterSize(FONT_SIZE);
        }
        else
        {
            m_static_pass->setCharacterSize(FONT_SIZE + 5);
            m_static_name->setCharacterSize(FONT_SIZE);
        }
    }
    break;
    case sf::Keyboard::Return:
    {
        if (m_textname->getString().getSize() > 0 && !m_strpass.empty())
        {
            login(m_textname->getString(), m_strpass);
            if (m_loginSuccess)
            {
                m_window.close();
                m_blink_run = false;
                m_blink_fut.wait();
            }
        }
    }
    break;
    case sf::Keyboard::Escape:
    {
        m_window.close();
        m_blink_run = false;
        m_blink_fut.wait();
    }
    break;
    default:
        break;
    }
}

void LoginWindow::blinkAnimation()
{
    std::unique_lock<std::mutex> lock(m_blink_mt, std::defer_lock);
    while (m_blink_run)
    {
        std::this_thread::sleep_for(500ms);

        lock.lock();
        m_focusedName.load() ? m_static_name->setCharacterSize(FONT_SIZE) : m_static_pass->setCharacterSize(FONT_SIZE);
        std::this_thread::sleep_for(500ms);
        m_focusedName.load() ? m_static_name->setCharacterSize(FONT_SIZE + 5)
                             : m_static_pass->setCharacterSize(FONT_SIZE + 5);
        lock.unlock();
    }
}
