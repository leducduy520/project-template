#include "soundplayer.hpp"
#include "constants.hpp"
#include "magic_enum.hpp"
#include "magic_enum_all.hpp"
#include <filesystem>
#include <regex>
#include <iostream>

SoundPlayer* SoundPlayer::m_instance = nullptr;
std::once_flag SoundPlayer::m_callflag;

SoundPlayer::SoundPlayer()
{
    // std::cout << "\nAll sounds in resource path:\n";
    // const std::regex txt_regex("[a-z]+\\.wav");
    // std::cmatch match;
    // for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{constants::resouces_path})
    // {
    //     if (std::regex_search(dir_entry.path().string().c_str(), match, txt_regex))
    //     {
    //         std::cout << dir_entry << '\n';
    //     }
    // }

    m_music.openFromFile(constants::resouces_path + "game_background.wav");
    m_music.setVolume(50.0F);
    m_music.setLoop(true);
    m_music.play();

    m_player.setVolume(50.0F);

    magic_enum::enum_for_each<SoundMode>([this](auto val) {
        constexpr SoundMode mode = val;
        sf::SoundBuffer buffer;
        switch (mode)
        {
        case SoundMode::WALL_BOUNCE:
        {
            if (buffer.loadFromFile(constants::resouces_path + "wall_bounce.wav"))
            {
                m_soundDict.emplace(mode, std::move(buffer));
            }
        }
        break;
        case SoundMode::BRICK_BOUNCE:
        {
            if (buffer.loadFromFile(constants::resouces_path + "brick_bounce.wav"))
            {
                m_soundDict.emplace(mode, std::move(buffer));
            }
        }
        break;
        case SoundMode::PADDLE_BOUNCE:
        {
            if (buffer.loadFromFile(constants::resouces_path + "paddle_bounce.wav"))
            {
                m_soundDict.emplace(mode, std::move(buffer));
            }
        }
        break;
        case SoundMode::DIAMOND_DESTROY:
        {
            if (buffer.loadFromFile(constants::resouces_path + "diamond_destroy.wav"))
            {
                m_soundDict.emplace(mode, std::move(buffer));
            }
        }
        break;
        case SoundMode::BOMB_EXPLOSION:
        {
            if (buffer.loadFromFile(constants::resouces_path + "bomb_explosion.wav"))
            {
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

void SoundPlayer::playSound(const SoundMode& mode)
{
    auto iter = m_soundDict.find(mode);
    if (iter != m_soundDict.end())
    {
        m_player.setBuffer(iter->second);
        m_player.play();
    }
}

//Some text in detached head
