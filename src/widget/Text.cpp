#include "Text.hpp"
#include <iostream>

namespace duyld
{
    Text::Text(Frame* parent, const sf::Vector2f& shape)
        : Frame(parent, shape)
    {
        m_layout = Frame::Horizontal;
    }

    void Text::setText(sf::Text &text)
    {
        m_text = &text;
    }

    void Text::update()
    {
        updateHorizontalLayout();
    }

    void Text::updateHorizontalLayout()
    {
        auto text_size_x = m_text->getGlobalBounds().getSize().x;
        auto text_size_y = m_text->getCharacterSize() * 1.0F;
        auto text_pos = m_text->getPosition();
        auto this_size = this->getSize();
        auto this_pos = this->getPosition();
        // std::cout << "this_size: " << this_size.x << " - " << this_size.y << std::endl;
        // std::cout << "text_size: " << text_size_x << " - " << text_size_y << std::endl;
        // std::cout << "text_pos: " << text_pos.x << " - " << text_pos.y << std::endl;
        // std::cout << "this_pos: " << this_pos.x << " - " << this_pos.y << std::endl;

        if (m_vertical_resizing == Hug)
        {
            this->setSize({this_size.x, text_size_y + m_vertical_pad.x + m_vertical_pad.y});
            this_size = this->getSize();
        }
        if (m_horizontal_resizing == Hug)
        {
            this->setSize({text_size_x + m_horizontal_pad.x + m_horizontal_pad.y, this_size.y});
            this_size = this->getSize();
        }
        if(this_size.x < text_size_x + m_horizontal_pad.x + m_horizontal_pad.y)
        {
            this->setSize({text_size_x + m_horizontal_pad.x + m_horizontal_pad.y, this_size.y});
            this_size = this->getSize();
        }
        if(this_size.y < text_size_y + m_vertical_pad.x + m_vertical_pad.y)
        {
            this->setSize({this_size.x, text_size_y + m_vertical_pad.x + m_vertical_pad.y});
            this_size = this->getSize();
        }

        // std::cout << "this_size: " << this_size.x << " - " << this_size.y << std::endl;
        // std::cout << "text_size: " << text_size_x << " - " << text_size_y << std::endl;
        // std::cout << "text_pos: " << text_pos.x << " - " << text_pos.y << std::endl;
        // std::cout << "this_pos: " << this_pos.x << " - " << this_pos.y << std::endl;
        switch (m_vertical_alignment)
        {
        case Alignment::LEFT:
        {
            //std::cout << "left\n";
            m_text->setPosition({this_pos.x + m_horizontal_pad.x, text_pos.y});
        }
        break;
        case Alignment::CENTER:
        {
            //std::cout << "center\n";
            m_text->setPosition({this_pos.x + m_horizontal_pad.x + (this_size.x - text_size_x)/2, text_pos.y});
        }
        break;
        case Alignment::RIGHT:
        {
            //std::cout << "right\n";
            m_text->setPosition({this_pos.x + m_horizontal_pad.x + (this_size.x - text_size_x), text_pos.y});
        }
        break;
        default:
            break;
        }

        text_pos = m_text->getPosition();
        // std::cout << "this_size: " << this_size.x << " - " << this_size.y << std::endl;
        // std::cout << "text_size: " << text_size_x << " - " << text_size_y << std::endl;
        // std::cout << "text_pos: " << text_pos.x << " - " << text_pos.y << std::endl;
        // std::cout << "this_pos: " << this_pos.x << " - " << this_pos.y << std::endl;

        switch (m_horizontal_alignment)
        {
        case Alignment::TOP:
        {
            //std::cout << "top\n";
            m_text->setPosition({text_pos.x, this_pos.y + m_vertical_pad.x});
        }
        break;
        case Alignment::MIDDLE:
        {
            //std::cout << "middle\n";
            m_text->setPosition({text_pos.x, this_pos.y + m_vertical_pad.x + (this_size.y - text_size_y) / 2});
        }
        break;
        case Alignment::BOTTOM:
        {
            //std::cout << "bottom\n";
            m_text->setPosition({text_pos.x, this_pos.y + m_vertical_pad.x + (this_size.y - text_size_y)});
        }
        break;
        default:
            break;
        }
        text_pos = m_text->getPosition();
        /*std::cout << "this_size: " << this_size.x << " - " << this_size.y << std::endl;
        std::cout << "text_size: " << text_size_x << " - " << text_size_y << std::endl;
        std::cout << "text_pos: " << text_pos.x << " - " << text_pos.y << std::endl;
        std::cout << "this_pos: " << this_pos.x << " - " << this_pos.y << std::endl;*/
    }

    void Text::draw(sf::RenderTarget &target, sf::RenderStates states)
    {
        Frame::draw(target, states);
        target.draw(*m_text, states);
    }

    Text::~Text()
    {
        delete m_text;
        m_text = nullptr;
    }

} // namespace duyld
