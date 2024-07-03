#ifndef __SOUNDPLAYER_H__
#define __SOUNDPLAYER_H__

#include "SFML/Audio.hpp"
#include <unordered_map>

class SoundPlayer {
public:
    static SoundPlayer* getInstance(){
        static auto* instance = new SoundPlayer();
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

    SoundPlayer (const SoundPlayer &other) = delete;
    SoundPlayer (SoundPlayer &&other) = delete;
    SoundPlayer &operator=(const SoundPlayer &other) = delete;
    SoundPlayer &operator=(SoundPlayer &&other) = delete;

    void playSound(const SoundMode& mode);

private:
    sf::Sound m_player;
    sf::Music m_music;
    std::unordered_map<SoundMode, sf::SoundBuffer> m_soundDict;
};

#endif // __SOUNDPLAYER_H__