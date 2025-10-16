#ifndef __SOUNDADAPTER_H__
#define __SOUNDADAPTER_H__

#include <string>
#include "constants.hpp"

enum class SoundMode
{
    WALL_BOUNCE,
    BRICK_BOUNCE,
    PADDLE_BOUNCE,
    DIAMOND_DESTROY,
    BOMB_EXPLOSION
};

struct SoundAdapter
{
    static std::string get_sound_file_path(const SoundMode& mode)
    {
        switch (mode)
        {
        case SoundMode::WALL_BOUNCE:
            return "wall_bounce.wav";
        case SoundMode::BRICK_BOUNCE:
            return "brick_bounce.wav";
        case SoundMode::PADDLE_BOUNCE:
            return "paddle_bounce.wav";
        case SoundMode::DIAMOND_DESTROY:
            return "diamond_destroy.wav";
        case SoundMode::BOMB_EXPLOSION:
            return "bomb_explosion.wav";
        default:
            return "";
        }
    };

    static std::string get_sound_mode_id(const SoundMode& mode)
    {
        switch (mode)
        {
        case SoundMode::WALL_BOUNCE:
            return "WALL_BOUNCE";
        case SoundMode::BRICK_BOUNCE:
            return "BRICK_BOUNCE";
        case SoundMode::PADDLE_BOUNCE:
            return "PADDLE_BOUNCE";
        case SoundMode::DIAMOND_DESTROY:
            return "DIAMOND_DESTROY";
        case SoundMode::BOMB_EXPLOSION:
            return "BOMB_EXPLOSION";
        default:
            return "UNKNOWN_SOUND";
        }
    };

    static std::string get_background_sound_file_path()
    {
        return constants::resouces_path + "game_background.wav";
    }

    static std::string get_backgound_sound_id()
    {
        return "GAME_BACKGROUND_MUSIC";
    }
};

#endif // __SOUNDADAPTER_H__
