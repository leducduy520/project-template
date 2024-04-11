#pragma once
#include <iostream>
#include <stdint.h>

using namespace std;

class Base
{
private:
    /* data */
    uint16_t x, y;

public:
    /**
     * @brief Construct a new Base object
     * 
     * @param X 
     * @param Y 
     * !asdad
     * TODO
     * decr
     */
    Base(const uint16_t &X, const uint16_t &Y);
    Base(const Base &other);
    const bool operator==(const Base &other)
    {
        if (this != &other)
        {
            return (x == other.x && y == other.y);
        }
        return true;
    }
    const bool operator!=(const Base &other)
    {
        if (this != &other)
        {
            return !(*this == other);
        }
        return false;
    }
    ~Base();
};
