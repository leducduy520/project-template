#include "base.hpp"

Base::Base(const uint16_t &Xparam, const uint16_t &Yparam) : x(Xparam), y(Yparam)
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
