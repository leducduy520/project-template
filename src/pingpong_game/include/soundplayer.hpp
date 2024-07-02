#ifndef __SOUNDPLAYER_H__
#define __SOUNDPLAYER__

#include "SFML/Audio.hpp"
#include <unordered_map>

class SoundPlayer {
public:
    static SoundPlayer* getInstance(){
        static SoundPlayer* instance = new SoundPlayer();
        return instance;
    }

    enum SoundMode{
        WALL_BOUNCE,
        BRICK_BOUNCE
    };

    ~SoundPlayer();

    void playSound(const SoundMode& mode);

private:
    sf::SoundBuffer buffer;
    sf::Sound sound;
};

#endif // __SOUNDPLAYER__