#ifndef __SOUNDPLAYER_H__
#define __SOUNDPLAYER_H__

#include "SFML/Audio.hpp"
#include <unordered_map>

class SoundPlayer {
public:
    static SoundPlayer* getInstance(){
        std::call_once(m_callflag, []() mutable {
            m_instance = new SoundPlayer();
        });
        return m_instance;
    }

    static void destroyInstance(){
        delete m_instance;
        m_instance = nullptr;
    }

    enum SoundMode{
        WALL_BOUNCE,
        BRICK_BOUNCE,
        PADDLE_BOUNCE,
        DIAMOND_DESTROY,
        BOMB_EXPLOSION
    };


    SoundPlayer (const SoundPlayer &other) = delete;
    SoundPlayer (SoundPlayer &&other) = delete;
    SoundPlayer &operator=(const SoundPlayer &other) = delete;
    SoundPlayer &operator=(SoundPlayer &&other) = delete;

    void playSound(const SoundMode& mode);

protected:
    ~SoundPlayer();

private:
    SoundPlayer();
    sf::Sound m_player;
    sf::Music m_music;
    std::unordered_map<SoundMode, sf::SoundBuffer> m_soundDict;
    static std::once_flag m_callflag;
    static SoundPlayer *m_instance;
};

#endif // __SOUNDPLAYER_H__