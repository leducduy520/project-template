#include "soundplayer.hpp"
#include "constants.hpp"
#include "magic_enum.hpp"
#include "magic_enum_all.hpp"

SoundPlayer::SoundPlayer()
{
    m_music.openFromFile(constants::resoucesPath + "game_background.wav");
    m_music.setLoop(true);
    m_music.play();
    
    magic_enum::enum_for_each<SoundMode>([this] (auto val) {
        constexpr SoundMode mode = val;
        sf::SoundBuffer buffer;
        sf::Sound sound;
        switch (mode)
        {
        case SoundMode::WALL_BOUNCE:
        {
            if(buffer.loadFromFile(constants::resoucesPath + "wall_bounce.wav"))
            {
                sound.setBuffer(buffer);
                m_soundDict.emplace(mode, std::move(buffer));
            }
        }
            break;
        case SoundMode::BRICK_BOUNCE:
        {
            if(buffer.loadFromFile(constants::resoucesPath + "brick_bounce.wav"))
            {
                sound.setBuffer(buffer);
                m_soundDict.emplace(mode, std::move(buffer));
            }
        }
            break;
        case SoundMode::PADDLE_BOUNCE:
        {
            if(buffer.loadFromFile(constants::resoucesPath + "paddle_bounce.wav"))
            {
                sound.setBuffer(buffer);
                m_soundDict.emplace(mode, std::move(buffer));
            }
        }
            break;
        case SoundMode::DIAMOND_DESTROY:
        {
            if(buffer.loadFromFile(constants::resoucesPath + "diamond_destroy.wav"))
            {
                sound.setBuffer(buffer);
                m_soundDict.emplace(mode, std::move(buffer));
            }
        }
            break;
        case SoundMode::BOMB_EXPLOSION:
        {
            if(buffer.loadFromFile(constants::resoucesPath + "bomb_explosion.wav"))
            {
                sound.setBuffer(buffer);
                m_soundDict.emplace(mode, std::move(buffer));
            }
        }
            break;
        default:
            break;
        }
        
    });
}

SoundPlayer::~SoundPlayer()
{
    m_player.resetBuffer();
    m_soundDict.clear();
    m_music.stop();
}

void SoundPlayer::playSound(const SoundMode &mode)
{
    auto iter = m_soundDict.find(mode);
    if(iter != m_soundDict.end())
    {
        m_player.setBuffer(iter->second);
        m_player.play();
    }
}
