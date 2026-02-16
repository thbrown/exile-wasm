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
	// Web build: Web Audio API implementation
	#include <string>
	#include <emscripten.h>
	#include "compat/time.hpp"

	namespace sf {
		// Sound buffer - holds audio data
		class SoundBuffer {
		private:
			std::string filename_;

		public:
			SoundBuffer() {}

			bool loadFromFile(const std::string& filename) {
				filename_ = filename;
				// Audio should already be preloaded by init_snd_tool()
				// Just store the filename for playback
				return true;
			}

			bool loadFromMemory(const void* data, size_t sizeInBytes) { return true; }
			const void* getSamples() const { return nullptr; }
			unsigned int getSampleCount() const { return 0; }
			unsigned int getSampleRate() const { return 44100; }
			unsigned int getChannelCount() const { return 2; }
			Time getDuration() const { return Time(); }

			const std::string& getFilename() const { return filename_; }
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

			void play() {
				if (!buffer_) {
					EM_ASM({ console.warn('Sound::play() called but buffer is null'); });
					return;
				}

				const std::string& filename = buffer_->getFilename();
				EM_ASM_({
					var filename = UTF8ToString($0);
					console.log('Attempting to play sound: ' + filename);

					if (!Module.audioContext) {
						console.error('AudioContext not initialized');
						return;
					}

					if (Module.audioContext.state === 'suspended') {
						console.warn('AudioContext is suspended, attempting to resume...');
						Module.audioContext.resume();
					}

					if (!Module.audioBuffers[filename]) {
						console.error('Audio buffer not loaded: ' + filename);
						console.log('Available buffers:', Object.keys(Module.audioBuffers));
						return;
					}

					try {
						var source = Module.audioContext.createBufferSource();
						source.buffer = Module.audioBuffers[filename];
						source.connect(Module.audioContext.destination);
						source.start(0);
						console.log('Sound played successfully: ' + filename);
					} catch(e) {
						console.error('Failed to play sound: ' + filename, e);
					}
				}, filename.c_str());
			}

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
			// Always return Stopped for WASM so channels are immediately freed
			// Web Audio plays sounds fire-and-forget, no need to track
			Status getStatus() const { return Status::Stopped; }
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

		// Audio initialization - preload all sounds
		inline void initAudio() {
			EM_ASM({
				if (!Module.audioContext) {
					Module.audioContext = new (window.AudioContext || window.webkitAudioContext)();
					Module.audioBuffers = {};
					Module.audioLoadCount = 0;
					Module.audioTotalCount = 0;
				}
			});
		}

		// Preload a sound file
		inline void preloadSound(const std::string& filename) {
			EM_ASM_({
				var filename = UTF8ToString($0);
				if (!Module.audioContext) {
					console.warn('Cannot preload sound - AudioContext not initialized: ' + filename);
					return;
				}

				Module.audioTotalCount++;

				try {
					var data = FS.readFile(filename);
					console.log('Decoding audio: ' + filename + ' (' + data.length + ' bytes)');
					Module.audioContext.decodeAudioData(
						data.buffer.slice(data.byteOffset, data.byteOffset + data.byteLength),
						function(buffer) {
							Module.audioBuffers[filename] = buffer;
							Module.audioLoadCount++;
							console.log('Decoded: ' + filename + ' (' + Module.audioLoadCount + '/' + Module.audioTotalCount + ')');
							if (Module.audioLoadCount === Module.audioTotalCount) {
								console.log('All audio files loaded (' + Module.audioLoadCount + ')');
							}
						},
						function(err) {
							Module.audioLoadCount++;
							console.error('Failed to decode: ' + filename, err);
						}
					);
				} catch(e) {
					Module.audioLoadCount++;
					console.error('Failed to read from filesystem: ' + filename, e);
				}
			}, filename.c_str());
		}
	}

#else
	// Desktop build: Use actual SFML
	#include <SFML/Audio.hpp>
#endif

#endif // BOE_COMPAT_AUDIO_HPP
