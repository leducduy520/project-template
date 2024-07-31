#include "constants.hpp"
#include "DBClientGame.hpp"
#include "LoginGame.hpp"

#define INPUT_BOUND_WIDTH 200
#define INPUT_BOUND_HEIGHT 50
#define FONT_SIZE 30
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

void LoginWindow::centeredText(sf::Text &text, const sf::Vector2f &bound_size, const sf::Vector2f &bound_pos)
{
    if(bound_size.x > constants::window_width || bound_size.y > constants::window_height)
    {
        return;
    }
    
    auto textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    text.setPosition(bound_pos.x + bound_size.x / 2.0f,
                      bound_pos.y + bound_size.y / 2.0f);
}

void LoginWindow::login(const std::string &username, const std::string &password)
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
            throw std::runtime_error("It's not correct password for user "s + username );
        }
        if (DBINSTANCE->InsertDocument(
                make_document(kvp("name", username), kvp("password", password))))
        {
            cout << "successfully insert initial data for " << username << '\n';
            m_loginSuccess = true;
            return;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

LoginWindow::LoginWindow() : m_focusedName(true), m_loginSuccess(false), m_blink_run(false)
{
    m_window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Login Window");
    m_window.setPosition(sf::Vector2i{(1920 - WINDOW_WIDTH) / 2, (1080 - WINDOW_HEIGHT) / 2});
    m_window.setFramerateLimit(120);

    m_font.loadFromFile(constants::resoucesPath + "Cross Boxed.ttf");

    m_textname.setFont(m_font);
    m_textname.setFillColor(sf::Color::White);
    m_textname.setCharacterSize(FONT_SIZE);

    m_textpass = m_textname;

    m_static_name = m_textname;
    m_static_name.setString("User name: ");
    m_static_name.setFillColor(sf::Color::Red);
    auto textRect = m_static_name.getLocalBounds();
    m_static_name.setOrigin(0, textRect.top + textRect.height / 2.0f);
    m_static_name.setPosition(100, INPUT_BOUND_HEIGHT / 2 + 100);

    m_static_pass = m_textname;
    m_static_pass.setString("User password: ");
    m_static_pass.setOrigin(0, textRect.top + textRect.height / 2.0f);
    m_static_pass.setPosition(100, INPUT_BOUND_HEIGHT / 2 + 200);

    m_blink_run = true;
    m_blink_fut = std::async(std::launch::async, &LoginWindow::blinkAnimation, this);
}

std::pair<bool, std::string> LoginWindow::run()
{
    while(m_window.isOpen())
    {
        listening();
        update();
        render();
    }
    return std::make_pair(m_loginSuccess, m_strname);
}

void LoginWindow::listening()
{
    sf::Event event{};
    while(m_window.pollEvent(event))
    {
        if(event.type == sf::Event::TextEntered)
        {
            auto code = event.text.unicode;
            // wcout << code << ": " << static_cast<char>(code) << '\n';
            if(code < 128 && code != 0x09)
            {
                updateText(code);
            }
        }
        if(event.type == sf::Event::KeyPressed)
        {
            handleKeyPress(event);
        }
    }
}

void LoginWindow::update()
{
    /*if (m_focusedName)
    {
        m_static_name.setFillColor(sf::Color::Red);
        m_static_pass.setFillColor(sf::Color::White);
    }
    else
    {
        m_static_pass.setFillColor(sf::Color::Red);
        m_static_name.setFillColor(sf::Color::White);
    }
    centeredText(m_textname, sf::Vector2f{200, 50}, sf::Vector2f{400, 100});
    centeredText(m_textpass, sf::Vector2f{200, 50}, sf::Vector2f{400, 200});*/
}

void LoginWindow::render()
{
    if(m_window.isOpen())
    {
        m_window.clear(sf::Color::Black);
        m_window.draw(m_static_name);
        m_window.draw(m_static_pass);
        m_window.draw(m_textname);
        m_window.draw(m_textpass);
        m_window.display();
    }
}

void LoginWindow::updateText(const sf::Uint32 &code)
{
    sf::String str = m_focusedName.load() ? m_textname.getString() : m_textpass.getString();

    if (str.getSize() > 0 && (code == 0x7F || code == 0x08))
    {
        str.erase(str.getSize() - 1);
        if (m_focusedName.load())
        {
            m_textname.setString(str);
            m_textname.setFillColor(sf::Color::Red);
            m_strname = str;
            centeredText(m_textname,
                         sf::Vector2f{INPUT_BOUND_WIDTH, INPUT_BOUND_HEIGHT},
                         sf::Vector2f{400, 100});
        }
        else
        {
            m_textpass.setString(str);
            m_textpass.setFillColor(sf::Color::Red);
            m_strpass = str;
            centeredText(m_textpass,
                         sf::Vector2f{INPUT_BOUND_WIDTH, INPUT_BOUND_HEIGHT},
                         sf::Vector2f{400, 200});
        }
    }
    else if (m_focusedName.load() && code > 20)
    {
        str += static_cast<char>(code);
        m_strname = str;
        m_textname.setString(str);
        m_textname.setFillColor(sf::Color::Red);
        centeredText(m_textname,
                     sf::Vector2f{INPUT_BOUND_WIDTH, INPUT_BOUND_HEIGHT},
                     sf::Vector2f{400, 100});
    }
    else if (code > 20)
    {
        str += '*';
        m_strpass += static_cast<char>(code);
        m_textpass.setString(str);
        m_textpass.setFillColor(sf::Color::Red);
        centeredText(m_textpass,
                     sf::Vector2f{INPUT_BOUND_WIDTH, INPUT_BOUND_HEIGHT},
                     sf::Vector2f{400, 200});
    }
}

void LoginWindow::handleKeyPress(const sf::Event &event)
{
    switch (event.key.code)
    {
    case sf::Keyboard::Tab:
    {
        m_focusedName.store(!m_focusedName.load());
        if (m_focusedName.load())
        {
            m_textname.setFillColor(sf::Color::Red);
            m_textpass.setFillColor(sf::Color::White);

            m_static_name.setFillColor(sf::Color::Red);
            m_static_pass.setFillColor(sf::Color::White);
        }
        else
        {
            m_textname.setFillColor(sf::Color::White);
            m_textpass.setFillColor(sf::Color::Red);
            
            m_static_pass.setFillColor(sf::Color::Red);
            m_static_name.setFillColor(sf::Color::White);
        }
    }
    break;
    case sf::Keyboard::Return:
    {
        if (!m_strname.empty() && !m_strpass.empty())
        {
            login(m_strname, m_strpass);
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
    while (m_blink_run)
    {
        std::unique_lock<std::mutex> lock(m_blink_mt, std::defer_lock);
        std::this_thread::sleep_for(500ms);

        lock.lock();
        m_focusedName.load() ? m_static_name.setFillColor(sf::Color::White) : m_static_pass.setFillColor(sf::Color::White);
        std::this_thread::sleep_for(500ms);
        m_focusedName.load() ? m_static_name.setFillColor(sf::Color::Red) : m_static_pass.setFillColor(sf::Color::Red);
    }
}
