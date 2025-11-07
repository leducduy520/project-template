#ifndef SOUNDMANAGER_HPP
#define SOUNDMANAGER_HPP

#include <SFML/Audio.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include <string_view>
#include <memory>
#include <mutex>

class SoundManager {
public:
    // Get the single instance of SoundManager
    static SoundManager& getInstance(size_t poolSize = 20, float defaultVolume = 100.0f);

    // Delete copy constructor and assignment operator
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

    // Load a sound effect from a file
    void loadSoundEffect(std::string_view name, std::string_view filePath);

    // Load music from a file
    void loadMusic(std::string_view name, std::string_view filePath);

    // Play a sound effect using the pool
    void playSoundEffect(std::string_view name);

    // Play music
    void playMusic(std::string_view name, bool loop = true);

    // Stop music
    void stopMusic(std::string_view name);

    // Pause music
    void pauseMusic(std::string_view name);

    // Stop all sounds
    void stopAllSounds();

    // Pause all sounds
    void pauseAllSounds();

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

    std::unordered_map<std::string, sf::SoundBuffer> soundBuffers; // Stores sound buffers
    std::vector<sf::Sound> soundPool; // Pool of reusable sound objects
    std::unordered_map<std::string, std::unique_ptr<sf::Music>> musicTracks; // Stores music tracks
    float globalVolume; // Global volume level (0-100)
    size_t maxConcurrentSounds; // Maximum number of concurrent sounds
    mutable std::mutex mutex; // Mutex for thread safety
};

struct SoundPlayer
{
    enum class Sound_t {
        WALL_BOUNCE,
        BRICK_BOUNCE,
        PADDLE_BOUNCE,
        DIAMOND_DESTROY,
        BOMB_EXPLOSION
    };
    static void loadSound();
    static void stopSound();
    static void playSound(Sound_t type);
private:
    struct sound_id {
        static constexpr std::string_view WALL_BOUNCE_ID = "wall_bounce_t";
        static constexpr std::string_view BRICK_BOUNCE_ID = "brick_bounce_t";
        static constexpr std::string_view PADDLE_BOUNCE_ID = "paddle_bounce_t";
        static constexpr std::string_view DIAMOND_DESTROY_ID = "diamond_destroy_t";
        static constexpr std::string_view BOMB_EXPLOSION_ID = "bomb_explosion_t";
    };
};


#endif // SOUNDMANAGER_HPP
