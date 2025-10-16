#ifndef SOUNDMANAGER_HPP
#define SOUNDMANAGER_HPP

#include <SFML/Audio.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include "soundadapter.hpp"

class SoundManager
{
public:
    // Get the single instance of SoundManager
    static SoundManager& getInstance(size_t poolSize = 20, float defaultVolume = 100.0f);

    // Delete copy constructor and assignment operator
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

    // Load a sound effect from a file
    void loadSoundEffect(const std::string& name, const std::string& filePath);

    // Load music from a file
    void loadMusic(const std::string& name, const std::string& filePath);

    // Play a sound effect using the pool
    void playSoundEffect(const std::string& name);

    // Play music
    void playMusic(const std::string& name, bool loop = true);

    // Stop music
    void stopMusic(const std::string& name);

    // Pause music
    void pauseMusic(const std::string& name);

    // Set global volume (0.0f to 100.0f)
    void setGlobalVolume(float volume);

    // Get global volume
    float getGlobalVolume() const;

    // Clean up stopped sounds
    void cleanupSounds();

private:
    // Private constructor to enforce Singleton
    SoundManager(size_t poolSize, float defaultVolume);

    // Destructor
    ~SoundManager();

    std::unordered_map<std::string, sf::SoundBuffer> soundBuffers;           // Stores sound buffers
    std::vector<sf::Sound> soundPool;                                        // Pool of reusable sound objects
    std::unordered_map<std::string, std::unique_ptr<sf::Music>> musicTracks; // Stores music tracks
    float globalVolume;                                                      // Global volume level (0-100)
    size_t maxConcurrentSounds;                                              // Maximum number of concurrent sounds
    mutable std::mutex mutex;                                                // Mutex for thread safety
};

#endif // SOUNDMANAGER_HPP
