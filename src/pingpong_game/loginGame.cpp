#include "constants.hpp"
#include "dbclientGame.hpp"
#include "loginGame.hpp"


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

LoginWindow::LoginWindow() : m_focusedName(true), m_loginSuccess(false), m_id(-1)
{
    m_window.create(sf::VideoMode(800, 600), "Login Window");
    m_window.setFramerateLimit(120);

    m_font.loadFromFile(constants::resoucesPath + "Cross Boxed.ttf");

    m_textname.setFont(m_font);
    m_textname.setFillColor(sf::Color::White);
    m_textname.setCharacterSize(30);

    m_textpass = m_textname;

    m_static_name = m_textname;
    m_static_name.setString("User name: ");
    m_static_pass = m_textname;
    m_static_pass.setString("User password: ");

    m_static_name.setOrigin(0, m_static_name.getLocalBounds().height / 2);
    m_static_name.setPosition(sf::Vector2f{100, 100});
    m_static_pass.setOrigin(0, m_static_pass.getLocalBounds().height / 2);
    m_static_pass.setPosition(sf::Vector2f{100, 200});
}

void LoginWindow::login(const std::string username, const std::string password)
{
    try
    {
        int64_t userid = std::hash<std::string>{}(username);
        DBINSTANCE->GetDatabase("duyld");
        if (!DBINSTANCE->GetCollection("pingpong_game"))
        {
            DBINSTANCE->CreateCollection("pingpong_game");
            if (DBINSTANCE->InsertDocument(make_document(kvp("name", username),kvp("id", userid), kvp("password", password))))
            {
                cout << "successfully insert initial data for " << username << endl;
                m_loginSuccess = true;
                m_id = userid;
            }
            throw std::runtime_error("Failed to insert initial data for "s + username);
        }
        if(DBINSTANCE->GetDocument(make_document(kvp("id", userid))))
        {
            cout << "User " << username << " exists" << endl;
            if (DBINSTANCE->GetDocument(make_document(kvp("id", userid), kvp("password", password))))
            {
                cout << "User " << username << " login successfully\n";
                m_loginSuccess = true;
                m_id = userid;
            }
            throw std::runtime_error("It's not correct password for user "s + username );
        }
        if (DBINSTANCE->InsertDocument(
                make_document(kvp("name", username), kvp("id", userid), kvp("password", password))))
        {
            cout << "successfully insert initial data for " << username << endl;
            m_loginSuccess = true;
            m_id = userid;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

std::pair<bool, int64_t> LoginWindow::run()
{
    while(m_window.isOpen())
    {
        eventHandler();
        update();
        render();
    }
    return std::make_pair(m_loginSuccess, m_id);
}

void LoginWindow::eventHandler()
{
    sf::Event event{};
    while(m_window.pollEvent(event))
    {
        if(event.type == sf::Event::TextEntered)
        {
            auto code = event.text.unicode;
            cout << code << endl;
            if(code < 128 && code != 0x09)
            {
                
                sf::String str = m_focusedName ? m_textname.getString() : m_textpass.getString();

                if (str.getSize() > 0 && (code == 0x7F || code == 0x08))
                {
                    str.erase(str.getSize() - 1);
                    m_focusedName ? m_textname.setString(str) : m_textpass.setString(str);
                    m_focusedName ? (m_strname = str) : (m_strpass = str);
                }
                else if(m_focusedName)
                {
                    str += static_cast<char>(code);
                    m_strname = str;
                    m_textname.setString(str);
                }
                else
                {
                    str += '*';
                    m_strpass += static_cast<char>(code);
                    m_textpass.setString(str);
                }
            }
        }
        if(event.type == sf::Event::KeyPressed)
        {
            switch(event.key.code)
            {
            case sf::Keyboard::Tab:
            {
                m_focusedName = !m_focusedName;
            }
            break;
            case sf::Keyboard::Return:
            {
                login(m_strname, m_strpass);
                if (m_loginSuccess)
                {
                    m_window.close();
                }
            }
            break;
            case sf::Keyboard::Escape:
            {
                m_window.close();
            }
            break;
            default:
                break;
            }
            
        }
    }
}

void LoginWindow::update()
{
    if (m_focusedName)
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
    centeredText(m_textpass, sf::Vector2f{200, 50}, sf::Vector2f{400, 200});
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
