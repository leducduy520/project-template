#ifndef __IGAME__
#define __IGAME__

#include <string>

class IGame{
    public:
	virtual void run() = 0;
    virtual void init(std::string) = 0;
    ~IGame() = default;
};

extern "C" {
    __declspec(dllexport) IGame* createPingPongGame();
    __declspec(dllexport) void destroyGame(IGame* game);
}

#endif // __IGAME__