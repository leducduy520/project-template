#include "soundplayer.hpp"
#include "constants.hpp"

SoundPlayer::~SoundPlayer()
{
    sound.resetBuffer();
}

void SoundPlayer::playSound(const SoundMode &mode)
{
    std::string filepath;
    switch(mode)
    {
        case SoundMode::WALL_BOUNCE:
        {
            filepath.assign(std::move(constants::resoucesPath + "bouncing.wav"));
        }
            break;
        case SoundMode::BRICK_BOUNCE:
        {
            filepath.assign(std::move(constants::resoucesPath + "brick.wav"));
        }
            break;
        default:
            break;
    }
    if(buffer.loadFromFile(filepath))
    {
        sound.setBuffer(buffer);
        sound.play();
    }
}
