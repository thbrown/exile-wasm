/*
 * input_stream.hpp
 * Compatibility layer for SFML InputStream
 *
 * For WebAssembly builds, provides simple InputStream interface
 * For desktop builds, uses SFML InputStream
 */

#ifndef BOE_COMPAT_INPUT_STREAM_HPP
#define BOE_COMPAT_INPUT_STREAM_HPP

#ifdef __EMSCRIPTEN__
	// Web build: Simple InputStream interface
	#include <cstdint>

	namespace sf {
		// Type aliases for SFML integer types
		typedef int64_t Int64;
		typedef int32_t Int32;
		typedef int16_t Int16;
		typedef int8_t  Int8;
		typedef uint64_t Uint64;
		typedef uint32_t Uint32;
		typedef uint16_t Uint16;
		typedef uint8_t  Uint8;

		// Abstract input stream interface (matches SFML API)
		class InputStream {
		public:
			virtual ~InputStream() {}

			// Read data from the stream
			// Returns the number of bytes actually read, or -1 on error
			virtual Int64 read(void* data, Int64 size) = 0;

			// Change the current reading position
			// Returns the position actually sought to, or -1 on error
			virtual Int64 seek(Int64 position) = 0;

			// Get the current reading position in the stream
			// Returns the current position, or -1 on error
			virtual Int64 tell() = 0;

			// Return the size of the stream
			// Returns the total number of bytes available, or -1 on error
			virtual Int64 getSize() = 0;
		};
	}

#else
	// Desktop build: Use actual SFML
	#include <SFML/System/InputStream.hpp>
#endif

#endif // BOE_COMPAT_INPUT_STREAM_HPP
