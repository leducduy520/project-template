#include "SoundManager.hpp"
#include <stdexcept>

SoundManager& SoundManager::getInstance(size_t poolSize, float defaultVolume)
{
    static SoundManager instance(poolSize, defaultVolume);
    return instance;
}

SoundManager::SoundManager(size_t poolSize, float defaultVolume)
    : globalVolume(defaultVolume), maxConcurrentSounds(poolSize)
{
    // Pre-allocate sound pool
    soundPool.reserve(poolSize);
    for (size_t i = 0; i < poolSize; ++i)
    {
        soundPool.emplace_back();
    }
}

SoundManager::~SoundManager()
{
    // Clear containers to free resources
    std::lock_guard<std::mutex> lock(mutex);
    soundBuffers.clear();
    soundPool.clear();
    musicTracks.clear();
}

void SoundManager::loadSoundEffect(const std::string& name, const std::string& filePath)
{
    std::lock_guard<std::mutex> lock(mutex);
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(filePath))
    {
        throw std::runtime_error("Failed to load sound effect: " + filePath);
    }
    soundBuffers[name] = std::move(buffer);
}

void SoundManager::loadMusic(const std::string& name, const std::string& filePath)
{
    std::lock_guard<std::mutex> lock(mutex);
    std::unique_ptr<sf::Music> music = std::make_unique<sf::Music>();
    if (!music->openFromFile(filePath))
    {
        throw std::runtime_error("Failed to load music: " + filePath);
    }
    musicTracks[name] = std::move(music);
}

void SoundManager::playSoundEffect(const std::string& name)
{
    std::lock_guard<std::mutex> lock(mutex);
    auto it = soundBuffers.find(name);
    if (it == soundBuffers.end())
    {
        throw std::runtime_error("Sound effect not found: " + name);
    }

    // Find an available sound object in the pool
    sf::Sound* availableSound = nullptr;
    for (auto& sound : soundPool)
    {
        if (sound.getStatus() == sf::Sound::Stopped)
        {
            availableSound = &sound;
            break;
        }
    }

    // If no available sound and pool isn't full, use a new slot
    if (!availableSound && soundPool.size() < maxConcurrentSounds)
    {
        soundPool.emplace_back();
        availableSound = &soundPool.back();
    }

    // If we found an available sound, configure and play it
    if (availableSound)
    {
        availableSound->setBuffer(it->second);
        availableSound->setVolume(globalVolume);
        availableSound->play();
    }
}

void SoundManager::playMusic(const std::string& name, bool loop)
{
    std::lock_guard<std::mutex> lock(mutex);
    auto it = musicTracks.find(name);
    if (it == musicTracks.end())
    {
        throw std::runtime_error("Music not found: " + name);
    }
    it->second->setLoop(loop);
    it->second->setVolume(globalVolume);
    it->second->play();
}

void SoundManager::stopMusic(const std::string& name)
{
    std::lock_guard<std::mutex> lock(mutex);
    auto it = musicTracks.find(name);
    if (it == musicTracks.end())
    {
        throw std::runtime_error("Music not found: " + name);
    }
    it->second->stop();
}

void SoundManager::pauseMusic(const std::string& name)
{
    std::lock_guard<std::mutex> lock(mutex);
    auto it = musicTracks.find(name);
    if (it == musicTracks.end())
    {
        throw std::runtime_error("Music not found: " + name);
    }
    it->second->pause();
}

void SoundManager::setGlobalVolume(float volume)
{
    std::lock_guard<std::mutex> lock(mutex);
    if (volume < 0.0f || volume > 100.0f)
    {
        throw std::runtime_error("Volume must be between 0 and 100");
    }
    globalVolume = volume;

    // Update volume for active sounds
    for (auto& sound : soundPool)
    {
        if (sound.getStatus() == sf::Sound::Playing)
        {
            sound.setVolume(globalVolume);
        }
    }

    // Update volume for playing music
    for (auto& music : musicTracks)
    {
        if (music.second->getStatus() == sf::Music::Playing)
        {
            music.second->setVolume(globalVolume);
        }
    }
}

float SoundManager::getGlobalVolume() const
{
    std::lock_guard<std::mutex> lock(mutex);
    return globalVolume;
}

void SoundManager::cleanupSounds()
{
    std::lock_guard<std::mutex> lock(mutex);
    // Remove excess stopped sounds if pool grows beyond max
    if (soundPool.size() > maxConcurrentSounds)
    {
        soundPool.erase(std::remove_if(soundPool.begin(),
                                       soundPool.end(),
                                       [](const sf::Sound& sound) { return sound.getStatus() == sf::Sound::Stopped; }),
                        soundPool.end());
    }
}
