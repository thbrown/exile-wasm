/*
 * sound_player.hpp
 * BoE - Platform Abstraction Layer for Audio
 *
 * Interface for platform-independent audio playback
 * Abstracts SFML Audio (desktop) and Web Audio API (web)
 */

#ifndef BOE_SOUND_PLAYER_HPP
#define BOE_SOUND_PLAYER_HPP

#include <string>
#include <memory>

// Forward declarations
namespace sf {
    class Sound;
    class SoundBuffer;
}

/**
 * Platform-independent sound interface
 */
class ISound {
public:
    virtual ~ISound() = default;

    // Playback control
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;

    // Volume control (0.0 to 100.0)
    virtual void setVolume(float volume) = 0;
    virtual float getVolume() const = 0;

    // Looping
    virtual void setLoop(bool loop) = 0;
    virtual bool getLoop() const = 0;

    // Status
    virtual bool isPlaying() const = 0;
};

/**
 * Platform-independent sound player interface
 */
class ISoundPlayer {
public:
    virtual ~ISoundPlayer() = default;

    // Load a sound from file
    virtual std::unique_ptr<ISound> loadSound(const std::string& filename) = 0;

    // Quick play (fire and forget)
    virtual void playSound(const std::string& filename, float volume = 100.0f) = 0;

    // Stop all sounds
    virtual void stopAll() = 0;

    // Global volume control (0.0 to 100.0)
    virtual void setGlobalVolume(float volume) = 0;
    virtual float getGlobalVolume() const = 0;
};

// Factory function for creating platform-specific implementations
std::unique_ptr<ISoundPlayer> createSoundPlayer();

#endif // BOE_SOUND_PLAYER_HPP
