#include "Frame.hpp"
#include <iostream>

namespace duyld
{
Frame::Frame(Frame *parent, const sf::Vector2f &shape)
    : sf::RectangleShape(shape) ,m_parent(parent), m_vertical_pad({0, 0}), m_horizontal_pad({0, 0}), m_vertical_gap(-1), m_horizontal_gap(-1),
    m_layout(Vertical), m_horizontal_resizing(Hug), m_vertical_resizing(Hug), m_vertical_alignment(LEFT), m_horizontal_alignment(MIDDLE)
{
}

void Frame::addChild(duyld::Frame* shape)
{
    m_children.push_back(shape);
}

void Frame::setLayout(Frame::Layout layout)
{
    m_layout = layout;
}

void Frame::setHorizontalResizing(Frame::Resizing resizing)
{
    m_horizontal_resizing = resizing;
}

void Frame::setVerticalResizing(Frame::Resizing resizing)
{
    m_vertical_resizing = resizing;
}

void Frame::setPadding(const sf::Vector2f &vertical, const sf::Vector2f &horizontal)
{
    m_vertical_pad = vertical;
    m_horizontal_pad = horizontal;
}

void Frame::setHorizontalPadding(const sf::Vector2f &horizontal)
{
    m_horizontal_pad = horizontal;
}

void Frame::setVerticalPadding(const sf::Vector2f &vertical)
{
    m_vertical_pad = vertical;
}

void Frame::setGaps(sf::Int16 vertical, sf::Int16 horizontal)
{
    m_vertical_gap = vertical;
    m_horizontal_gap = horizontal;
}

void Frame::setHorizontalGap(sf::Int16 horizontal)
{
    m_horizontal_gap = horizontal;
}

void Frame::setVerticalGap(sf::Int16 vertical)
{
    m_vertical_gap = vertical;
}

void Frame::setHorizontalAligment(Frame::Alignment alignment)
{
    m_horizontal_alignment = alignment;
}

void Frame::setVerticalAligment(Frame::Alignment alignment)
{
    m_vertical_alignment = alignment;
}

void Frame::update()
{
    switch (m_layout)
    {
    case Layout::Vertical:
        updateVerticalLayout();
        break;
    case Layout::Horizontal:
        updateHorizontalLayout();
        break;
    case Layout::Wrap:
        updateWrapLayout();
        break;
    default:
        break;
    }
}
void Frame::updateVerticalLayout()
{
    float totol_height{.0F};
    float max_width{.0F};
    
    for (auto &child : m_children)
    {
        totol_height += child->getLocalBounds().getSize().y;
        if(child->getLocalBounds().getSize().x > max_width)
        {
            max_width = child->getLocalBounds().getSize().x;
        }
    }

    if(m_vertical_gap != -1.0F)
    {
        totol_height += m_vertical_gap * (m_children.size() - 1);
    }
    
    auto this_size = this->getSize();
    if(m_vertical_resizing == Hug)
    {
        this->setSize({this_size.x, totol_height + m_vertical_pad.x + m_vertical_pad.y});
        this_size = this->getSize();
    }
    if(m_horizontal_resizing == Hug)
    {
        this->setSize({max_width + m_horizontal_pad.x + m_horizontal_pad.y, this_size.y});
        this_size = this->getSize();
    }
    if(this_size.x < max_width + m_horizontal_pad.x + m_horizontal_pad.y)
    {
        this->setSize({max_width + m_horizontal_pad.x + m_horizontal_pad.y, this_size.y});
        this_size = this->getSize();
    }

    auto this_pos = this->getPosition();
    float init_y = this->getGlobalBounds().getPosition().y + m_vertical_pad.x;
    for (auto &child : m_children)
    {
        auto* child_alias = child;
        auto child_size = child_alias->getGlobalBounds().getSize();
        switch(m_vertical_alignment)
        {
            case Alignment::LEFT:
            {
                child_alias->setPosition({this_pos.x + m_horizontal_pad.x, init_y});
            }
            break;
            case Alignment::CENTER:
            {
                child_alias->setPosition({this_pos.x + (this_size.x - child_size.x)/2 + m_horizontal_pad.x, init_y});
            }
            break;
            case Alignment::RIGHT:
            {
                child_alias->setPosition({this_pos.x + (this_size.x - child_size.x) + m_horizontal_pad.x, init_y});
            }
            break;
            default: break;
        }
        child->update();
        (m_vertical_gap != -1.0F) ? (init_y += (m_vertical_gap + child_alias->getSize().y) ) : (init_y);
    }
}
void Frame::updateHorizontalLayout()
{
    float totol_width{.0F};
    float max_height{.0F};
    
    for (auto &child : m_children)
    {
        totol_width += child->getLocalBounds().getSize().x;
        if(child->getLocalBounds().getSize().y > max_height)
        {
            max_height = child->getLocalBounds().getSize().y;
        }
    }

    if(m_horizontal_gap != -1.0F)
    {
        totol_width += m_horizontal_gap * (m_children.size() - 1);
    }
    
    auto this_size = this->getSize();
    if(m_vertical_resizing == Hug)
    {
        this->setSize({this_size.x, max_height + m_vertical_pad.x + m_vertical_pad.y});
        this_size = this->getSize();
    }
    if(m_horizontal_resizing == Hug)
    {
        this->setSize({totol_width + m_horizontal_pad.x + m_horizontal_pad.y, this_size.y});
        this_size = this->getSize();
    }
    if(this_size.y < max_height + m_vertical_pad.x + m_vertical_pad.y)
    {
        this->setSize({this_size.x, max_height + m_vertical_pad.x + m_vertical_pad.y});
        this_size = this->getSize();
    }

    auto this_pos = this->getPosition();
    float init_x = this->getGlobalBounds().getPosition().x + m_horizontal_pad.x;
    for (auto &child : m_children)
    {
        auto* child_alias = child;
        auto child_size = child_alias->getGlobalBounds().getSize();
        switch(m_horizontal_alignment)
        {
            case Alignment::TOP:
            {
                child_alias->setPosition({init_x, this_pos.y + m_vertical_pad.x});
            }
            break;
            case Alignment::MIDDLE:
            {
                child_alias->setPosition({init_x, this_pos.y + m_vertical_pad.x + (this_size.y - child_size.y)/2});
            }
            break;
            case Alignment::BOTTOM:
            {
                child_alias->setPosition({init_x, this_pos.y + m_vertical_pad.x + (this_size.y - child_size.y)});
            }
            break;
            default: break;
        }
        (m_horizontal_gap != -1.0F) ? (init_x += m_horizontal_gap) : (init_x);
    }
}
void Frame::updateWrapLayout()
{
}
void Frame::draw(sf::RenderTarget &target, sf::RenderStates states)
{
    target.draw(*this, states);
    for (const auto &child : m_children)
    {
        //std::cout << "child draw\n";
        child->draw(target, states);
    }
}
Frame::~Frame()
{
    for(auto &child : m_children)
    {
        delete child;
        child = nullptr;
    }
}
} // namespace duyld
