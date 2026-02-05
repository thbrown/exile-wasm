/*
 * time.hpp
 * Compatibility layer for SFML Time types
 *
 * For WebAssembly builds, provides simple time implementation
 * For desktop builds, uses SFML Time
 */

#ifndef BOE_COMPAT_TIME_HPP
#define BOE_COMPAT_TIME_HPP

#ifdef __EMSCRIPTEN__
	// Web build: Simple time implementation
	#include <chrono>

	namespace sf {
		class Time {
		private:
			std::chrono::microseconds duration;

		public:
			Time() : duration(0) {}
			explicit Time(std::chrono::microseconds us) : duration(us) {}

			float asSeconds() const {
				return std::chrono::duration<float>(duration).count();
			}

			int asMilliseconds() const {
				return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
			}

			long long asMicroseconds() const {
				return duration.count();
			}

			static Time seconds(float amount) {
				return Time(std::chrono::duration_cast<std::chrono::microseconds>(
					std::chrono::duration<float>(amount)));
			}

			static Time milliseconds(int amount) {
				return Time(std::chrono::duration_cast<std::chrono::microseconds>(
					std::chrono::milliseconds(amount)));
			}

			static Time microseconds(long long amount) {
				return Time(std::chrono::microseconds(amount));
			}

			// Comparison operators
			bool operator==(const Time& other) const { return duration == other.duration; }
			bool operator!=(const Time& other) const { return duration != other.duration; }
			bool operator<(const Time& other) const { return duration < other.duration; }
			bool operator>(const Time& other) const { return duration > other.duration; }
			bool operator<=(const Time& other) const { return duration <= other.duration; }
			bool operator>=(const Time& other) const { return duration >= other.duration; }

			// Arithmetic operators
			Time operator+(const Time& other) const { return Time(duration + other.duration); }
			Time operator-(const Time& other) const { return Time(duration - other.duration); }
			Time& operator+=(const Time& other) { duration += other.duration; return *this; }
			Time& operator-=(const Time& other) { duration -= other.duration; return *this; }
		};

		// Clock for timing (similar to sf::Clock)
		class Clock {
		private:
			std::chrono::steady_clock::time_point start_time;

		public:
			Clock() : start_time(std::chrono::steady_clock::now()) {}

			Time getElapsedTime() const {
				auto now = std::chrono::steady_clock::now();
				auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - start_time);
				return Time(elapsed);
			}

			Time restart() {
				auto now = std::chrono::steady_clock::now();
				auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - start_time);
				start_time = now;
				return Time(elapsed);
			}
		};

		// Sleep function (stub for web builds)
		inline void sleep(Time duration) {
			// Stub - in real web implementation would use emscripten_sleep
			// For now, just do nothing
		}

		// Free function aliases for convenience
		inline Time seconds(float amount) { return Time::seconds(amount); }
		inline Time milliseconds(int amount) { return Time::milliseconds(amount); }
		inline Time microseconds(long long amount) { return Time::microseconds(amount); }
	}

#else
	// Desktop build: Use actual SFML
	#include <SFML/System/Time.hpp>
	#include <SFML/System/Clock.hpp>
#endif

#endif // BOE_COMPAT_TIME_HPP
