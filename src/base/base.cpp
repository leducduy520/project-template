#include "base.hpp"

Base::Base(const uint16_t &X, const uint16_t &Y) : x(X), y(Y)
{
}

Base::Base(const Base &other)
{
    if (this != &other)
    {
        x = other.x;
        y = other.y;
    }
}
Base::~Base()
{
}
