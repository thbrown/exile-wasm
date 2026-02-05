/*
 * audio.hpp
 * Compatibility layer for SFML Audio
 *
 * For WebAssembly builds, provides stub audio implementation (TODO: Web Audio API)
 * For desktop builds, uses SFML Audio
 */

#ifndef BOE_COMPAT_AUDIO_HPP
#define BOE_COMPAT_AUDIO_HPP

#ifdef __EMSCRIPTEN__
	// Web build: Stub audio for now (TODO: Implement Web Audio API)
	#include <string>
	#include "compat/time.hpp"

	namespace sf {
		// Sound buffer - holds audio data
		class SoundBuffer {
		public:
			SoundBuffer() {}
			bool loadFromFile(const std::string& filename) { return true; } // Stub
			bool loadFromMemory(const void* data, size_t sizeInBytes) { return true; }
			const void* getSamples() const { return nullptr; }
			unsigned int getSampleCount() const { return 0; }
			unsigned int getSampleRate() const { return 44100; }
			unsigned int getChannelCount() const { return 2; }
			Time getDuration() const { return Time(); }
		};

		// Sound - plays a sound buffer
		class Sound {
		public:
			// Sound status enum (nested like in SFML)
			enum Status {
				Stopped,
				Paused,
				Playing
			};

		private:
			const SoundBuffer* buffer_ = nullptr;
			float volume_ = 100.f;
			float pitch_ = 1.f;
			bool loop_ = false;
			Status status_ = Status::Stopped;

		public:
			Sound() {}
			explicit Sound(const SoundBuffer& buffer) : buffer_(&buffer) {}

			void play() { status_ = Status::Playing; }
			void pause() { status_ = Status::Paused; }
			void stop() { status_ = Status::Stopped; }

			void setBuffer(const SoundBuffer& buffer) { buffer_ = &buffer; }
			void setLoop(bool loop) { loop_ = loop; }
			void setVolume(float volume) { volume_ = volume; }
			void setPitch(float pitch) { pitch_ = pitch; }
			void setPlayingOffset(Time timeOffset) {}

			const SoundBuffer* getBuffer() const { return buffer_; }
			bool getLoop() const { return loop_; }
			float getVolume() const { return volume_; }
			float getPitch() const { return pitch_; }
			Status getStatus() const { return status_; }
			Time getPlayingOffset() const { return Time(); }
		};

		// Music - streams audio from file
		class Music {
		public:
			using Status = Sound::Status;

		private:
			float volume_ = 100.f;
			float pitch_ = 1.f;
			bool loop_ = false;
			Status status_ = Status::Stopped;

		public:
			Music() {}

			bool openFromFile(const std::string& filename) { return true; } // Stub

			void play() { status_ = Status::Playing; }
			void pause() { status_ = Status::Paused; }
			void stop() { status_ = Status::Stopped; }

			void setLoop(bool loop) { loop_ = loop; }
			void setVolume(float volume) { volume_ = volume; }
			void setPitch(float pitch) { pitch_ = pitch; }
			void setPlayingOffset(Time timeOffset) {}

			bool getLoop() const { return loop_; }
			float getVolume() const { return volume_; }
			float getPitch() const { return pitch_; }
			Status getStatus() const { return status_; }
			Time getPlayingOffset() const { return Time(); }
			Time getDuration() const { return Time(); }
		};

		// Listener - 3D audio listener (not used in 2D game, stub it)
		namespace Listener {
			inline void setGlobalVolume(float volume) {}
			inline float getGlobalVolume() { return 100.f; }
		}
	}

#else
	// Desktop build: Use actual SFML
	#include <SFML/Audio.hpp>
#endif

#endif // BOE_COMPAT_AUDIO_HPP
