#ifndef __IGAME__
#define __IGAME__

#include <string>

class IGame
{
public:
    virtual void run() = 0;
    virtual void init(std::string&) = 0;
    virtual ~IGame() = default;
};

#if defined _WIN32
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API
#endif

extern "C"
{
    EXPORT_API IGame* createPingPongGame();
}

#endif // __IGAME__
