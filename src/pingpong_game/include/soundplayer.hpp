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
        BRICK_BOUNCE,
        PADDLE_BOUNCE,
        DIAMOND_DESTROY,
        BOMB_EXPLOSION
    };

    SoundPlayer();
    ~SoundPlayer();

    void playSound(const SoundMode& mode);

private:
    sf::Sound m_player;
    sf::Music m_music;
    std::unordered_map<SoundMode, sf::SoundBuffer> m_soundDict;
};

#endif // __SOUNDPLAYER__