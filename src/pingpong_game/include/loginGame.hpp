#ifndef __LOGIN_GAME__
#define __LOGIN_GAME__

#include "SFML/Graphics.hpp"

class LoginWindow{
    sf::RenderWindow m_window;
    sf::Text m_textname;
    sf::Text m_textpass;
    sf::Text m_static_name;
    sf::Text m_static_pass;
    sf::Font m_font;
    std::string m_strname;
    std::string m_strpass;
    bool m_focusedName;
    bool m_loginSuccess;
    static void centeredText(sf::Text &text, const sf::Vector2f &bound_size, const sf::Vector2f &bound_pos);
    void login(const std::string& username, const std::string& password);
    void listening();
    void update();
    void render();
    void updateText(const sf::Uint32& code);
    void handleKeyPress(const sf::Event& event);
public:
    LoginWindow();
    std::pair<bool, std::string> run();
};

#endif //   __LOGIN_GAME__  //
