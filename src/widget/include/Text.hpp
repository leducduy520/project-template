#ifndef __WIDGET_TEXT__
#define __WIDGET_TEXT__

#include "Frame.hpp"

namespace duyld{
class Text: public Frame
{
public:
    Text(Frame* parent = nullptr, const sf::Vector2f& shape = sf::Vector2f{0, 0});
    void setText(sf::Text& text);
    void update() override;
    void updateHorizontalLayout() override;
    void checkingChildUpdate() override;
    void draw(sf::RenderTarget& target, sf::RenderStates states) override;
    ~Text();
private:
    sf::Text* m_text;
};
}

#endif  // __WIDGET_TEXT__
