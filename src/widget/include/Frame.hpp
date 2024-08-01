#ifndef __WIDGET_FRAME__
#define __WIDGET_FRAME__

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <list>


namespace duyld{
    
    class Frame;
    // using ListChild = std::list<duyld::Frame*>;

    class Frame : public sf::RectangleShape
    {
    public:
        enum Layout
        {
            Horizontal,
            Vertical,
            Wrap
        };

        enum Resizing
        {
            Fixed,
            Hug
        };

        enum Alignment
        {
            CENTER,
            LEFT,
            RIGHT,
            MIDDLE,
            TOP,
            BOTTOM
        };

        explicit Frame(Frame *parent = nullptr, const sf::Vector2f &shape = sf::Vector2f{0, 0});
        virtual void addChild(std::unique_ptr<Frame> shape);
        virtual void setLayout(Frame::Layout layout);
        virtual void setHorizontalResizing(Frame::Resizing resizing);
        virtual void setVerticalResizing(Frame::Resizing resizing);
        virtual void setPadding(const sf::Vector2f &vertical, const sf::Vector2f &horizontal);
        virtual void setHorizontalPadding(const sf::Vector2f &horizontal);
        virtual void setVerticalPadding(const sf::Vector2f &vertical);
        virtual void setGaps(sf::Int16 vertical, sf::Int16 horizontal);
        virtual void setHorizontalGap(sf::Int16 horizontal);
        virtual void setVerticalGap(sf::Int16 vertical);
        virtual void setHorizontalAligment(Frame::Alignment alignment);
        virtual void setVerticalAligment(Frame::Alignment alignment);
        virtual void update();
        virtual void updateVerticalLayout();
        virtual void updateHorizontalLayout();
        virtual void updateWrapLayout();
        virtual void draw(sf::RenderTarget &target, sf::RenderStates states);
        virtual ~Frame();

    
    using ListChild = std::list<std::unique_ptr<Frame>>;

    protected:
        ListChild m_children;
        Frame *m_parent;
        sf::Vector2f m_vertical_pad;
        sf::Vector2f m_horizontal_pad;
        sf::Int16 m_vertical_gap;
        sf::Int16 m_horizontal_gap;
        Layout m_layout;
        Resizing m_horizontal_resizing;
        Resizing m_vertical_resizing;
        Alignment m_horizontal_alignment;
        Alignment m_vertical_alignment;
    };
}

#endif // __WIDGET_FRAME__
