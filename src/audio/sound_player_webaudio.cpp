/*
 * sound_player_webaudio.cpp
 * BoE - Web Audio API Implementation of Sound Player
 *
 * STUB IMPLEMENTATION - Phase 2
 * Will be fully implemented in Phase 5
 */

#include "sound_player.hpp"
#include <emscripten.h>
#include <map>
#include <vector>

/**
 * Web Audio implementation of ISound
 */
class WebAudioSound : public ISound {
private:
    int soundId; // JavaScript AudioBuffer ID
    float volume;
    bool loop;
    bool playing;

public:
    WebAudioSound(int id) : soundId(id), volume(100.0f), loop(false), playing(false) {}
    virtual ~WebAudioSound() {
        // TODO Phase 5: Clean up audio buffer in JavaScript
    }

    void play() override {
        // TODO Phase 5: Play sound via Web Audio API
        // EM_ASM({ playSound($0); }, soundId);
        playing = true;
    }

    void pause() override {
        // TODO Phase 5: Pause sound
        playing = false;
    }

    void stop() override {
        // TODO Phase 5: Stop sound
        // EM_ASM({ stopSound($0); }, soundId);
        playing = false;
    }

    void setVolume(float vol) override {
        volume = vol;
        // TODO Phase 5: Update volume in Web Audio
        // EM_ASM({ setSoundVolume($0, $1); }, soundId, vol / 100.0f);
    }

    float getVolume() const override {
        return volume;
    }

    void setLoop(bool l) override {
        loop = l;
        // TODO Phase 5: Update loop in Web Audio
    }

    bool getLoop() const override {
        return loop;
    }

    bool isPlaying() const override {
        return playing;
    }
};

/**
 * Web Audio implementation of ISoundPlayer
 */
class WebAudioPlayer : public ISoundPlayer {
private:
    std::map<std::string, int> loadedSounds; // filename -> soundId
    float globalVolume;

public:
    WebAudioPlayer() : globalVolume(100.0f) {
        // TODO Phase 5: Initialize Web Audio context
        // EM_ASM({ initWebAudio(); });
    }

    virtual ~WebAudioPlayer() = default;

    std::unique_ptr<ISound> loadSound(const std::string& filename) override {
        // TODO Phase 5: Load sound from Emscripten VFS
        // int soundId = EM_ASM_INT({ return loadSoundFile(UTF8ToString($0)); }, filename.c_str());
        int soundId = -1;

        if (soundId < 0) {
            return nullptr;
        }

        loadedSounds[filename] = soundId;
        return std::make_unique<WebAudioSound>(soundId);
    }

    void playSound(const std::string& filename, float volume = 100.0f) override {
        // TODO Phase 5: Quick play sound
        // auto sound = loadSound(filename);
        // if (sound) {
        //     sound->setVolume(volume * globalVolume / 100.0f);
        //     sound->play();
        // }
    }

    void stopAll() override {
        // TODO Phase 5: Stop all playing sounds
        // EM_ASM({ stopAllSounds(); });
    }

    void setGlobalVolume(float volume) override {
        globalVolume = volume;
        // TODO Phase 5: Update global volume
        // EM_ASM({ setGlobalVolume($0); }, volume / 100.0f);
    }

    float getGlobalVolume() const override {
        return globalVolume;
    }
};
