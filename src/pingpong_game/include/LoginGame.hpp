#ifndef __LOGIN_GAME__
#define __LOGIN_GAME__

#include "SFML/Graphics.hpp"
#include "Frame.hpp"
#include "Text.hpp"
#include <mutex>
#include <future>
#include <thread>

class LoginWindow{
    sf::RenderWindow m_window;
    sf::Text* m_textname;
    sf::Text* m_textpass;
    sf::Text* m_static_name;
    sf::Text* m_static_pass;
    duyld::Frame m_frame_login;
    sf::Font m_font;
    /*std::string m_strname;*/
    std::string m_strpass;
    std::atomic_bool m_focusedName;
    bool m_loginSuccess;

    std::future<void> m_blink_fut;
    std::mutex m_blink_mt;
    bool m_blink_run;


    static void centeredText(sf::Text &text, const sf::Vector2f &bound_size, const sf::Vector2f &bound_pos);
    void login(const std::string& username, const std::string& password);
    void listening();
    void update();
    void render();
    void updateText(const sf::Uint32& code);
    void handleKeyPress(const sf::Event& event);
    void blinkAnimation();

public:
    LoginWindow();
    std::pair<bool, std::string> run();
};

#endif //   __LOGIN_GAME__  //
