#ifndef SOUNDMANAGER_HPP
#define SOUNDMANAGER_HPP

#include <SFML/Audio.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include <string_view>
#include <memory>
#include <mutex>

/**
 * @brief Thread-safe singleton manager for audio resources (sound effects and music)
 * 
 * SoundManager provides centralized management of audio resources using SFML's audio system.
 * It implements a singleton pattern with a pre-allocated sound pool for efficient sound effect
 * playback, and supports both short sound effects and longer music tracks.
 * 
 * Key features:
 * - Singleton pattern with thread-safe initialization
 * - Pre-allocated sound pool to avoid runtime allocation overhead
 * - Global volume control for all sounds
 * - Thread-safe operations using mutex locks
 * - Explicit shutdown method for safe DLL unload
 * 
 * Usage example:
 * @code
 *   // Initialize with custom pool size and volume
 *   auto& manager = SoundManager::getInstance(10, 80.0f);
 *   
 *   // Load sound effects
 *   manager.loadSoundEffect("bounce", "sounds/bounce.wav");
 *   
 *   // Play sound effect (uses pool)
 *   manager.playSoundEffect("bounce");
 *   
 *   // Load and play music
 *   manager.loadMusic("background", "music/bg.ogg");
 *   manager.playMusic("background", true); // loop = true
 *   
 *   // Cleanup before DLL unload
 *   SoundManager::shutdown();
 * @endcode
 * 
 * @note This class is thread-safe. All public methods use mutex locks internally.
 * @warning Call shutdown() before DLL unload to prevent hangs during static destruction.
 */
class SoundManager
{
public:
    /**
     * @brief Get the singleton instance of SoundManager
     * 
     * Returns a reference to the single SoundManager instance. The instance is created
     * on first call using the provided parameters. Subsequent calls with different
     * parameters will use the initially created instance (parameters are ignored).
     * 
     * @param poolSize Number of pre-allocated sound objects in the pool (default: 20)
     * @param defaultVolume Initial global volume level, 0.0f to 100.0f (default: 100.0f)
     * @return Reference to the singleton SoundManager instance
     * 
     * @note Thread-safe: Uses C++11 static local variable initialization guarantee
     */
    static SoundManager& getInstance(size_t poolSize = 20, float defaultVolume = 100.0f);

    // Delete copy constructor and assignment operator (Singleton pattern)
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

    /**
     * @brief Load a sound effect from a file into memory
     * 
     * Loads a sound file and stores it in the internal buffer map. The sound can
     * then be played using playSoundEffect(). Sound files are loaded into memory
     * and can be played multiple times efficiently.
     * 
     * @param name Unique identifier for the sound effect (used as key)
     * @param filePath Path to the sound file (WAV, OGG, FLAC formats supported)
     * 
     * @throws std::runtime_error if the file cannot be loaded or opened
     * 
     * @note Thread-safe: Uses mutex lock internally
     * @note If a sound with the same name already exists, it will be replaced
     */
    void loadSoundEffect(std::string_view name, std::string_view filePath);

    /**
     * @brief Load a music track from a file
     * 
     * Loads a music file for streaming playback. Music files are streamed from disk
     * rather than loaded entirely into memory, making them suitable for longer tracks.
     * 
     * @param name Unique identifier for the music track (used as key)
     * @param filePath Path to the music file (OGG, WAV, FLAC formats supported)
     * 
     * @throws std::runtime_error if the file cannot be opened or loaded
     * 
     * @note Thread-safe: Uses mutex lock internally
     * @note Music files are streamed, not loaded into memory
     * @note If music with the same name already exists, it will be replaced
     */
    void loadMusic(std::string_view name, std::string_view filePath);

    /**
     * @brief Play a sound effect using the sound pool
     * 
     * Plays a previously loaded sound effect using an available sound object from
     * the pre-allocated pool. If all sounds in the pool are playing, the sound
     * will not play (unless the pool can be expanded within maxConcurrentSounds).
     * 
     * @param name Identifier of the sound effect to play (must be loaded first)
     * 
     * @throws std::runtime_error if the sound effect hasn't been loaded
     * 
     * @note Thread-safe: Uses mutex lock internally
     * @note The sound plays at the current global volume level
     * @note Multiple instances of the same sound can play simultaneously if pool allows
     */
    void playSoundEffect(std::string_view name);

    /**
     * @brief Play a music track
     * 
     * Starts playback of a previously loaded music track. If the music is already
     * playing, this will restart it from the beginning.
     * 
     * @param name Identifier of the music track to play (must be loaded first)
     * @param loop If true, the music will loop indefinitely (default: true)
     * 
     * @throws std::runtime_error if the music track hasn't been loaded
     * 
     * @note Thread-safe: Uses mutex lock internally
     * @note The music plays at the current global volume level
     * @note Only one music track can play at a time (playing a new one stops the previous)
     */
    void playMusic(std::string_view name, bool loop = true);

    /**
     * @brief Stop a specific music track
     * 
     * Stops playback of the specified music track if it is currently playing.
     * 
     * @param name Identifier of the music track to stop
     * 
     * @throws std::runtime_error if the music track hasn't been loaded
     * 
     * @note Thread-safe: Uses mutex lock internally
     */
    void stopMusic(std::string_view name);

    /**
     * @brief Pause a specific music track
     * 
     * Pauses playback of the specified music track. Playback can be resumed
     * by calling playMusic() again (it will continue from where it was paused).
     * 
     * @param name Identifier of the music track to pause
     * 
     * @throws std::runtime_error if the music track hasn't been loaded
     * 
     * @note Thread-safe: Uses mutex lock internally
     */
    void pauseMusic(std::string_view name);

    /**
     * @brief Stop all currently playing sounds and music
     * 
     * Immediately stops all sound effects in the pool and all music tracks
     * that are currently playing. This is useful for game state transitions
     * or when pausing the game.
     * 
     * @note Thread-safe: Uses mutex lock internally
     * @note This does not unload the sounds/music, they can still be played again
     */
    void stopAllSounds();

    /**
     * @brief Pause all currently playing sounds and music
     * 
     * Pauses all sound effects in the pool and all music tracks that are
     * currently playing. Playback can be resumed by calling play methods again.
     * 
     * @note Thread-safe: Uses mutex lock internally
     * @note This does not unload the sounds/music
     */
    void pauseAllSounds();

    /**
     * @brief Set the global volume for all sounds and music
     * 
     * Sets a global volume level that applies to all sound effects and music tracks.
     * The volume is applied immediately to all currently playing sounds.
     * 
     * @param volume Volume level from 0.0f (silent) to 100.0f (maximum)
     * 
     * @throws std::runtime_error if volume is outside the valid range [0.0f, 100.0f]
     * 
     * @note Thread-safe: Uses mutex lock internally
     * @note Volume changes apply immediately to all playing sounds
     */
    void setGlobalVolume(float volume);

    /**
     * @brief Get the current global volume level
     * 
     * @return Current global volume level (0.0f to 100.0f)
     * 
     * @note Thread-safe: Uses mutex lock internally
     */
    float getGlobalVolume() const;

    /**
     * @brief Clean up stopped sounds from the pool
     * 
     * Removes excess stopped sound objects from the pool if it has grown beyond
     * maxConcurrentSounds. This helps manage memory usage if the pool expanded
     * dynamically during gameplay.
     * 
     * @note Thread-safe: Uses mutex lock internally
     * @note Only removes sounds that are in Stopped status
     */
    void cleanupSounds();

    /**
     * @brief Explicitly shutdown SoundManager before DLL unload
     * 
     * Stops all sounds, clears all containers, and marks the manager as shut down.
     * This method MUST be called before the plugin DLL is unloaded to prevent
     * hangs during static destruction when OpenAL threads cannot be properly joined.
     * 
     * This method:
     * - Stops all playing sounds and music
     * - Waits briefly for OpenAL to process stop commands
     * - Clears all sound buffers, sound pool, and music tracks
     * - Sets a flag to prevent double-cleanup in destructor
     * 
     * @note Thread-safe: Uses mutex lock internally
     * @note Safe to call multiple times (idempotent)
     * @warning Must be called before DLL unload in plugin architectures
     * 
     * @see SoundPlayer::stopSounds() - Convenience wrapper that calls this method
     */
    static void shutdown();

private:
    /**
     * @brief Private constructor to enforce Singleton pattern
     * 
     * @param poolSize Number of sound objects to pre-allocate in the pool
     * @param defaultVolume Initial global volume level (0.0f to 100.0f)
     */
    SoundManager(size_t poolSize, float defaultVolume);

    /**
     * @brief Destructor
     * 
     * If shutdown() was called, does nothing (cleanup already done).
     * Otherwise, performs minimal cleanup without waiting for threads
     * (safe for DLL unload scenarios).
     */
    ~SoundManager();

    std::unordered_map<std::string, sf::SoundBuffer>
        soundBuffers;                 ///< Map of loaded sound effect buffers (name -> buffer)
    std::vector<sf::Sound> soundPool; ///< Pre-allocated pool of reusable sound objects for efficient playback
    std::unordered_map<std::string, std::unique_ptr<sf::Music>>
        musicTracks;               ///< Map of loaded music tracks (name -> music)
    float globalVolume;            ///< Global volume level applied to all sounds (0.0f to 100.0f)
    size_t maxConcurrentSounds;    ///< Maximum number of concurrent sounds allowed in the pool
    mutable std::mutex mutex;      ///< Mutex for thread-safe access to all member variables
    static bool s_shutdown_called; ///< Flag indicating if shutdown() was called (prevents double-cleanup)
};

/**
 * @brief High-level sound player interface for game sound effects
 * 
 * Provides a simplified, type-safe API for loading and playing game sound effects.
 * This struct acts as a facade over SoundManager, providing game-specific sound
 * functionality with compile-time type safety through the SoundEffect_t enum.
 * 
 * Usage example:
 * @code
 *   SoundPlayer::loadSounds();           // Load all sound effects
 *   SoundPlayer::playSoundEffect(SoundPlayer::SoundEffect_t::WALL_BOUNCE);
 *   SoundPlayer::stopSounds();           // Stop all sounds and cleanup
 * @endcode
 */
struct SoundPlayer
{
    /**
     * @brief Enumeration of available game sound effect types
     * 
     * Each value corresponds to a specific sound effect used in the game.
     * Used with playSoundEffect() to play the corresponding sound.
     */
    enum class SoundEffect_t
    {
        WALL_BOUNCE,     ///< Sound played when ball bounces off wall
        BRICK_BOUNCE,    ///< Sound played when ball bounces off brick
        PADDLE_BOUNCE,   ///< Sound played when ball bounces off paddle
        DIAMOND_DESTROY, ///< Sound played when diamond brick is destroyed
        BOMB_EXPLOSION   ///< Sound played when bomb brick explodes
    };

    enum class Mussic_t
    {
        BACKGROUND
    };

    /**
     * @brief Load all game sound effects into SoundManager
     * 
     * Iterates through all SoundEffect_t enum values and loads the corresponding
     * sound files from the resources directory. Sound files are expected
     * to be in WAV format and located in constants::resouces_path.
     * 
     * Must be called before playSoundEffect() can be used.
     * 
     * @throws std::runtime_error if any sound file fails to load
     */
    static void loadSounds();

    /**
     * @brief Stop all sounds and shutdown SoundManager
     * 
     * Stops all currently playing sounds and explicitly shuts down the
     * SoundManager singleton. This should be called before DLL unload
     * to prevent hangs during static destruction.
     * 
     * This method calls SoundManager::shutdown() to ensure proper cleanup
     * of OpenAL resources before the plugin is unloaded.
     */
    static void stopSounds();

    /**
     * @brief Play a sound effect by type
     * 
     * Plays the specified sound effect using the SoundManager's sound pool.
     * The sound will be played at the current global volume level.
     * 
     * @param type The type of sound effect to play (from SoundEffect_t enum)
     * 
     * @throws std::runtime_error if the sound effect hasn't been loaded
     *         (call loadSounds() first)
     */
    static void playSoundEffect(SoundEffect_t type);

    /**
     * @brief Play a music by type
     * 
     * Plays the specified music using the SoundManager's music track.
     * The music will be played at the current global volume level.
     * 
     * @param type The type of music to play (from Mussic_t enum)
     */
    static void playMusic(Mussic_t type, bool loop = true);

private:
    /**
     * @brief Internal sound identifier constants
     * 
     * Contains string identifiers used to register and retrieve sound effects
     * in the SoundManager. These identifiers are used as keys in the
     * SoundManager's internal sound buffer map.
     */
    struct sound_id
    {
        static constexpr std::string_view WALL_BOUNCE_ID = "wall_bounce_t";     ///< Identifier for wall bounce sound
        static constexpr std::string_view BRICK_BOUNCE_ID = "brick_bounce_t";   ///< Identifier for brick bounce sound
        static constexpr std::string_view PADDLE_BOUNCE_ID = "paddle_bounce_t"; ///< Identifier for paddle bounce sound
        static constexpr std::string_view DIAMOND_DESTROY_ID =
            "diamond_destroy_t"; ///< Identifier for diamond destroy sound
        static constexpr std::string_view BOMB_EXPLOSION_ID =
            "bomb_explosion_t"; ///< Identifier for bomb explosion sound
    };

    /**
     * @brief Internal music identifier constants
     * 
     * Contains string identifiers used to register and retrieve music tracks
     * in the SoundManager. These identifiers are used as keys in the
     * SoundManager's internal music track map.
     */
    struct music_id
    {
        static constexpr std::string_view BACKGROUND_MUSIC_ID =
            "background_music_t"; ///< Identifier for background music
    };
};


#endif // SOUNDMANAGER_HPP
