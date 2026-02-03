/*
 * sound_player_sfml.cpp
 * BoE - SFML Implementation of Sound Player
 */

#include "sound_player_sfml.hpp"
#include <algorithm>

// ========== SfmlSound Implementation ==========

SfmlSound::SfmlSound(std::shared_ptr<sf::SoundBuffer> buf)
    : buffer(buf), sound(std::make_unique<sf::Sound>()) {
    sound->setBuffer(*buffer);
}

void SfmlSound::play() {
    sound->play();
}

void SfmlSound::pause() {
    sound->pause();
}

void SfmlSound::stop() {
    sound->stop();
}

void SfmlSound::setVolume(float volume) {
    sound->setVolume(volume);
}

float SfmlSound::getVolume() const {
    return sound->getVolume();
}

void SfmlSound::setLoop(bool loop) {
    sound->setLoop(loop);
}

bool SfmlSound::getLoop() const {
    return sound->getLoop();
}

bool SfmlSound::isPlaying() const {
    return sound->getStatus() == sf::Sound::Playing;
}

// ========== SfmlSoundPlayer Implementation ==========

SfmlSoundPlayer::SfmlSoundPlayer()
    : globalVolume(100.0f) {
}

std::unique_ptr<ISound> SfmlSoundPlayer::loadSound(const std::string& filename) {
    auto buffer = loadBuffer(filename);
    if (!buffer) {
        return nullptr;
    }

    return std::make_unique<SfmlSound>(buffer);
}

void SfmlSoundPlayer::playSound(const std::string& filename, float volume) {
    auto buffer = loadBuffer(filename);
    if (!buffer) {
        return;
    }

    auto sound = std::make_unique<SfmlSound>(buffer);
    sound->setVolume(volume * globalVolume / 100.0f);
    sound->play();

    activeSounds.push_back(std::move(sound));
    cleanupFinishedSounds();
}

void SfmlSoundPlayer::stopAll() {
    for (auto& sound : activeSounds) {
        sound->stop();
    }
    activeSounds.clear();
}

void SfmlSoundPlayer::setGlobalVolume(float volume) {
    globalVolume = std::max(0.0f, std::min(100.0f, volume));

    // Update all active sounds
    for (auto& sound : activeSounds) {
        float soundVol = sound->getVolume();
        sound->setVolume(soundVol * globalVolume / 100.0f);
    }
}

float SfmlSoundPlayer::getGlobalVolume() const {
    return globalVolume;
}

std::shared_ptr<sf::SoundBuffer> SfmlSoundPlayer::loadBuffer(const std::string& filename) {
    // Check if already loaded
    auto it = buffers.find(filename);
    if (it != buffers.end()) {
        return it->second;
    }

    // Load new buffer
    auto buffer = std::make_shared<sf::SoundBuffer>();
    if (!buffer->loadFromFile(filename)) {
        return nullptr;
    }

    buffers[filename] = buffer;
    return buffer;
}

void SfmlSoundPlayer::cleanupFinishedSounds() {
    // Remove sounds that have finished playing
    activeSounds.erase(
        std::remove_if(activeSounds.begin(), activeSounds.end(),
            [](const std::unique_ptr<SfmlSound>& sound) {
                return !sound->isPlaying();
            }),
        activeSounds.end()
    );
}

// ========== Factory Function ==========

std::unique_ptr<ISoundPlayer> createSoundPlayer() {
#ifdef USE_SFML
    return std::make_unique<SfmlSoundPlayer>();
#else
    // Web implementation will be created in Phase 5
    return nullptr;
#endif
}
