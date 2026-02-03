/*
 * sound_player_sfml.hpp
 * BoE - SFML Implementation of Sound Player
 *
 * Desktop (SFML) implementation of the audio abstraction layer
 */

#ifndef BOE_SOUND_PLAYER_SFML_HPP
#define BOE_SOUND_PLAYER_SFML_HPP

#include "sound_player.hpp"
#include <SFML/Audio.hpp>
#include <map>
#include <vector>
#include <memory>

/**
 * SFML implementation of ISound
 */
class SfmlSound : public ISound {
private:
    std::shared_ptr<sf::SoundBuffer> buffer;
    std::unique_ptr<sf::Sound> sound;

public:
    SfmlSound(std::shared_ptr<sf::SoundBuffer> buf);
    virtual ~SfmlSound() = default;

    // ISound interface
    void play() override;
    void pause() override;
    void stop() override;
    void setVolume(float volume) override;
    float getVolume() const override;
    void setLoop(bool loop) override;
    bool getLoop() const override;
    bool isPlaying() const override;

    // Direct access to SFML sound (for migration)
    sf::Sound& get() { return *sound; }
    const sf::Sound& get() const { return *sound; }
};

/**
 * SFML implementation of ISoundPlayer
 */
class SfmlSoundPlayer : public ISoundPlayer {
private:
    std::map<std::string, std::shared_ptr<sf::SoundBuffer>> buffers;
    std::vector<std::unique_ptr<SfmlSound>> activeSounds;
    float globalVolume;

public:
    SfmlSoundPlayer();
    virtual ~SfmlSoundPlayer() = default;

    // ISoundPlayer interface
    std::unique_ptr<ISound> loadSound(const std::string& filename) override;
    void playSound(const std::string& filename, float volume = 100.0f) override;
    void stopAll() override;
    void setGlobalVolume(float volume) override;
    float getGlobalVolume() const override;

private:
    std::shared_ptr<sf::SoundBuffer> loadBuffer(const std::string& filename);
    void cleanupFinishedSounds();
};

#endif // BOE_SOUND_PLAYER_SFML_HPP
