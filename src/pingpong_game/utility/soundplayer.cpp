#include "soundplayer.hpp"
#include <stdexcept>
#include <map>
#include <magic_enum/magic_enum_all.hpp>
#include "constants.hpp"

SoundManager& SoundManager::getInstance(size_t poolSize, float defaultVolume) {
    static SoundManager instance(poolSize, defaultVolume);
    return instance;
}

SoundManager::SoundManager(size_t poolSize, float defaultVolume)
    : globalVolume(defaultVolume), maxConcurrentSounds(poolSize) {
    // Pre-allocate sound pool
    soundPool.reserve(poolSize);
    for (size_t i = 0; i < poolSize; ++i) {
        soundPool.emplace_back();
    }
}

SoundManager::~SoundManager() {
    // Clear containers to free resources
    std::lock_guard<std::mutex> lock(mutex);
    soundBuffers.clear();
    soundPool.clear();
    musicTracks.clear();
}

void SoundManager::loadSoundEffect(std::string_view name, std::string_view filePath) {
    std::lock_guard<std::mutex> lock(mutex);
    sf::SoundBuffer buffer;
    std::string filePathStr(filePath);
    if (!buffer.loadFromFile(filePathStr)) {
        throw std::runtime_error("Failed to load sound effect: " + filePathStr);
    }
    soundBuffers[std::string(name)] = std::move(buffer);
}

void SoundManager::loadMusic(std::string_view name, std::string_view filePath) {
    std::lock_guard<std::mutex> lock(mutex);
    std::unique_ptr<sf::Music> music = std::make_unique<sf::Music>();
    std::string filePathStr(filePath);
    if (!music->openFromFile(filePathStr)) {
        throw std::runtime_error("Failed to load music: " + filePathStr);
    }
    musicTracks[std::string(name)] = std::move(music);
}

void SoundManager::playSoundEffect(std::string_view name) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = soundBuffers.find(std::string(name));
    if (it == soundBuffers.end()) {
        throw std::runtime_error("Sound effect not found: " + std::string(name));
    }

    // Find an available sound object in the pool
    sf::Sound* availableSound = nullptr;
    for (auto& sound : soundPool) {
        if (sound.getStatus() == sf::Sound::Stopped) {
            availableSound = &sound;
            break;
        }
    }

    // If no available sound and pool isn't full, use a new slot
    if (!availableSound && soundPool.size() < maxConcurrentSounds) {
        soundPool.emplace_back();
        availableSound = &soundPool.back();
    }

    // If we found an available sound, configure and play it
    if (availableSound) {
        availableSound->setBuffer(it->second);
        availableSound->setVolume(globalVolume);
        availableSound->play();
    }
}

void SoundManager::playMusic(std::string_view name, bool loop) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = musicTracks.find(std::string(name));
    if (it == musicTracks.end()) {
        throw std::runtime_error("Music not found: " + std::string(name));
    }
    it->second->setLoop(loop);
    it->second->setVolume(globalVolume);
    it->second->play();
}

void SoundManager::stopMusic(std::string_view name) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = musicTracks.find(std::string(name));
    if (it == musicTracks.end()) {
        throw std::runtime_error("Music not found: " + std::string(name));
    }
    it->second->stop();
}

void SoundManager::pauseMusic(std::string_view name) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = musicTracks.find(std::string(name));
    if (it == musicTracks.end()) {
        throw std::runtime_error("Music not found: " + std::string(name));
    }
    it->second->pause();
}

void SoundManager::stopAllSounds()
{
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& sound : soundPool) {
        sound.stop();
    }
    for (auto& music : musicTracks) {
        music.second->stop();
    }
}

void SoundManager::pauseAllSounds()
{
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& sound : soundPool) {
        sound.pause();
    }
    for (auto& music : musicTracks) {
        music.second->pause();
    }
}

void SoundManager::setGlobalVolume(float volume) {
    std::lock_guard<std::mutex> lock(mutex);
    if (volume < 0.0f || volume > 100.0f) {
        throw std::runtime_error("Volume must be between 0 and 100");
    }
    globalVolume = volume;

    // Update volume for active sounds
    for (auto& sound : soundPool) {
        if (sound.getStatus() == sf::Sound::Playing) {
            sound.setVolume(globalVolume);
        }
    }

    // Update volume for playing music
    for (auto& music : musicTracks) {
        if (music.second->getStatus() == sf::Music::Playing) {
            music.second->setVolume(globalVolume);
        }
    }
}

float SoundManager::getGlobalVolume() const {
    std::lock_guard<std::mutex> lock(mutex);
    return globalVolume;
}

void SoundManager::cleanupSounds() {
    std::lock_guard<std::mutex> lock(mutex);
    // Remove excess stopped sounds if pool grows beyond max
    if (soundPool.size() > maxConcurrentSounds) {
        soundPool.erase(
            std::remove_if(soundPool.begin(), soundPool.end(),
                [](const sf::Sound& sound) { return sound.getStatus() == sf::Sound::Stopped; }),
            soundPool.end());
    }
}

void SoundPlayer::loadSound()
{
    magic_enum::enum_for_each<Sound_t>([](auto val) {
        constexpr Sound_t mode = val;
        sf::SoundBuffer buffer;
        switch (mode)
        {
        case Sound_t::WALL_BOUNCE:
        {
            SoundManager::getInstance().loadSoundEffect(sound_id::WALL_BOUNCE_ID, (constants::resouces_path / "wall_bounce.wav").string());
        }
        break;
        case Sound_t::BRICK_BOUNCE:
        {
            SoundManager::getInstance().loadSoundEffect(sound_id::BRICK_BOUNCE_ID, (constants::resouces_path / "brick_bounce.wav").string());
        }
        break;
        case Sound_t::PADDLE_BOUNCE:
        {
            SoundManager::getInstance().loadSoundEffect(sound_id::PADDLE_BOUNCE_ID, (constants::resouces_path / "paddle_bounce.wav").string());
        }
        break;
        case Sound_t::DIAMOND_DESTROY:
        {
            SoundManager::getInstance().loadSoundEffect(sound_id::DIAMOND_DESTROY_ID, (constants::resouces_path / "diamond_destroy.wav").string());
        }
        break;
        case Sound_t::BOMB_EXPLOSION:
        {
            SoundManager::getInstance().loadSoundEffect(sound_id::BOMB_EXPLOSION_ID, (constants::resouces_path / "bomb_explosion.wav").string());
        }
        break;
        default:
            break;
        }
    });    
}

void SoundPlayer::stopSound()
{
    SoundManager::getInstance().stopAllSounds();
    SoundManager::getInstance().cleanupSounds();
}

void SoundPlayer::playSound(SoundPlayer::Sound_t type)
{
    static const std::map<SoundPlayer::Sound_t, std::string_view> sound_map{
        {Sound_t::WALL_BOUNCE, sound_id::WALL_BOUNCE_ID},
        {Sound_t::BRICK_BOUNCE, sound_id::BRICK_BOUNCE_ID},
        {Sound_t::PADDLE_BOUNCE, sound_id::PADDLE_BOUNCE_ID},
        {Sound_t::DIAMOND_DESTROY, sound_id::DIAMOND_DESTROY_ID},
        {Sound_t::BOMB_EXPLOSION, sound_id::BOMB_EXPLOSION_ID}
    };
    SoundManager::getInstance().playSoundEffect(sound_map.at(type));
}
