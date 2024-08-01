#include "constants.hpp"
#include "DBClientGame.hpp"
#include "LoginGame.hpp"
#include "Text.hpp"

#define INPUT_BOUND_WIDTH 200
#define INPUT_BOUND_HEIGHT 50
#define FONT_SIZE 32
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

    m_font.loadFromFile(constants::resoucesPath + "ModesticSans/ModesticSans-BoldItalic.ttf");

    using duyld::Frame;

    m_textname = new sf::Text();
    m_textname->setFillColor(sf::Color::Black);
    m_textname->setCharacterSize(FONT_SIZE);
    m_textname->setFont(m_font);

    m_textpass = new sf::Text();
    m_textpass->setFillColor(sf::Color::Black);
    m_textpass->setCharacterSize(FONT_SIZE);
    m_textpass->setFont(m_font);

    auto fram_text_name = new duyld::Text();
    fram_text_name->setFillColor(sf::Color::Transparent);
    fram_text_name->setVerticalAligment(Frame::CENTER);
    fram_text_name->setHorizontalAligment(Frame::MIDDLE);
    fram_text_name->setHorizontalResizing(Frame::Fixed);
    fram_text_name->setVerticalResizing(Frame::Fixed);
    fram_text_name->setHorizontalPadding({16, 0});
    fram_text_name->setSize({400, 50});
    fram_text_name->setText(*m_textname);
    fram_text_name->update();

    auto frame_text_pass = new duyld::Text();
    frame_text_pass->setFillColor(sf::Color::Transparent);
    frame_text_pass->setVerticalAligment(Frame::CENTER);
    frame_text_pass->setHorizontalAligment(Frame::MIDDLE);
    frame_text_pass->setHorizontalResizing(Frame::Fixed);
    frame_text_pass->setVerticalResizing(Frame::Fixed);
    frame_text_pass->setHorizontalPadding({16, 0});
    frame_text_pass->setSize({400, 50});
    frame_text_pass->setText(*m_textpass);
    frame_text_pass->update();

    auto frame_input = new Frame();
    frame_input->setFillColor(sf::Color::Transparent);
    frame_input->setLayout(Frame::Vertical);
    frame_input->setVerticalAligment(Frame::LEFT);
    frame_input->setHorizontalResizing(Frame::Fixed);
    frame_input->setVerticalGap(30);
    frame_input->setSize({200, 0});
    frame_input->setPosition(300, 235);
    frame_input->addChild(unique_ptr<Frame>(fram_text_name));
    frame_input->addChild(unique_ptr<Frame>(frame_text_pass));
    frame_input->update();

    m_static_name = new sf::Text();
    m_static_name->setString("Name: ");
    m_static_name->setFillColor(sf::Color::Black);
    m_static_name->setCharacterSize(FONT_SIZE);
    m_static_name->setFont(m_font);

    m_static_pass = new sf::Text();
    m_static_pass->setString("Password: ");
    m_static_pass->setFillColor(sf::Color::Black);
    m_static_pass->setCharacterSize(FONT_SIZE);
    m_static_pass->setFont(m_font);

    auto frame_title_name = new duyld::Text();
    frame_title_name->setFillColor(sf::Color::Transparent);
    frame_title_name->setVerticalAligment(Frame::LEFT);
    frame_title_name->setHorizontalAligment(Frame::MIDDLE);
    frame_title_name->setHorizontalResizing(Frame::Fixed);
    frame_title_name->setVerticalResizing(Frame::Fixed);
    frame_title_name->setHorizontalPadding({16,0});
    frame_title_name->setSize({200, 50});
    frame_title_name->setText(*m_static_name);
    frame_title_name->update();

    auto frame_title_pass = new duyld::Text();
    frame_title_pass->setFillColor(sf::Color::Transparent);
    frame_title_pass->setVerticalAligment(Frame::LEFT);
    frame_title_pass->setHorizontalAligment(Frame::MIDDLE);
    frame_title_pass->setHorizontalResizing(Frame::Fixed);
    frame_title_pass->setVerticalResizing(Frame::Fixed);
    frame_title_pass->setHorizontalPadding({16,0});
    frame_title_pass->setSize({200, 50});
    frame_title_pass->setText(*m_static_pass);
    frame_title_pass->update();

    auto frame_title = new Frame();
    frame_title->setFillColor(sf::Color::Transparent);
    frame_title->setLayout(Frame::Vertical);
    frame_title->setVerticalAligment(Frame::LEFT);
    frame_title->setHorizontalResizing(Frame::Fixed);
    frame_title->setVerticalGap(30);
    frame_title->setSize({200, 0});
    frame_title->setPosition(100, 235);
    frame_title->addChild(unique_ptr<Frame>(frame_title_name));
    frame_title->addChild(unique_ptr<Frame>(frame_title_pass));
    frame_title->update();

    m_frame_login.setFillColor(sf::Color::Transparent);
    m_frame_login.setLayout(Frame::Horizontal);
    m_frame_login.setHorizontalAligment(Frame::TOP);
    m_frame_login.setPosition(100, 235);
    m_frame_login.setHorizontalGap(0);
    m_frame_login.addChild(unique_ptr<Frame>(frame_title));
    m_frame_login.addChild(unique_ptr<Frame>(frame_input));
    m_frame_login.update();


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
    return std::make_pair(m_loginSuccess, m_textname->getString());
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
        m_static_name->setFillColor(sf::Color::Red);
        m_static_pass->setFillColor(sf::Color::Black);
    }
    else
    {
        m_static_pass->setFillColor(sf::Color::Red);
        m_static_name->setFillColor(sf::Color::Black);
    }
    centeredText(m_textname, sf::Vector2f{200, 50}, sf::Vector2f{400, 100});
    centeredText(m_textpass, sf::Vector2f{200, 50}, sf::Vector2f{400, 200});*/
}

void LoginWindow::render()
{
    if(m_window.isOpen())
    {
        m_window.clear(sf::Color{223,228, 210, 255});
        m_frame_login.draw(m_window, {});
        m_window.display();
    }
}

void LoginWindow::updateText(const sf::Uint32 &code)
{
    sf::String str = m_focusedName.load() ? m_textname->getString() : m_textpass->getString();

    if (str.getSize() > 0 && (code == 0x7F || code == 0x08))
    {
        str.erase(str.getSize() - 1);
        if (m_focusedName.load())
        {
            m_textname->setString(str);
            m_textname->setFillColor(sf::Color::Red);
            //m_strname = str;
            /*centeredText(m_textname,
                         sf::Vector2f{INPUT_BOUND_WIDTH, INPUT_BOUND_HEIGHT},
                         sf::Vector2f{400, 100});*/
        }
        else
        {
            m_textpass->setString(str);
            m_textpass->setFillColor(sf::Color::Red);
            m_strpass = str;
            /*centeredText(m_textpass,
                         sf::Vector2f{INPUT_BOUND_WIDTH, INPUT_BOUND_HEIGHT},
                         sf::Vector2f{400, 200});*/
        }
    }
    else if (m_focusedName.load() && code > 20)
    {
        str += static_cast<char>(code);
        //m_strname = str;
        m_textname->setString(str);
        m_textname->setFillColor(sf::Color::Red);
        /*centeredText(m_textname,
                     sf::Vector2f{INPUT_BOUND_WIDTH, INPUT_BOUND_HEIGHT},
                     sf::Vector2f{400, 100});*/
    }
    else if (code > 20)
    {
        str += '*';
        m_strpass += static_cast<char>(code);
        m_textpass->setString(str);
        m_textpass->setFillColor(sf::Color::Red);
        /*centeredText(m_textpass,
                     sf::Vector2f{INPUT_BOUND_WIDTH, INPUT_BOUND_HEIGHT},
                     sf::Vector2f{400, 200});*/
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
            m_textname->setFillColor(sf::Color::Red);
            m_textpass->setFillColor(sf::Color::Black);

            m_static_name->setFillColor(sf::Color::Red);
            m_static_pass->setFillColor(sf::Color::Black);
        }
        else
        {
            m_textname->setFillColor(sf::Color::Black);
            m_textpass->setFillColor(sf::Color::Red);
            
            m_static_pass->setFillColor(sf::Color::Red);
            m_static_name->setFillColor(sf::Color::Black);
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
    while (m_blink_run)
    {
        std::unique_lock<std::mutex> lock(m_blink_mt, std::defer_lock);
        std::this_thread::sleep_for(500ms);

        lock.lock();
        m_focusedName.load() ? m_static_name->setFillColor(sf::Color::Black) : m_static_pass->setFillColor(sf::Color::Black);
        std::this_thread::sleep_for(500ms);
        m_focusedName.load() ? m_static_name->setFillColor(sf::Color::Red) : m_static_pass->setFillColor(sf::Color::Red);
    }
}
