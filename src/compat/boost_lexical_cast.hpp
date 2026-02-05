/*
 * boost_lexical_cast.hpp
 * Compatibility shim for boost::lexical_cast
 *
 * For WebAssembly builds, provides std-based implementation
 */

#ifndef BOE_COMPAT_BOOST_LEXICAL_CAST_HPP
#define BOE_COMPAT_BOOST_LEXICAL_CAST_HPP

#ifdef __EMSCRIPTEN__
	// Web build: Use std::to_string and related functions
	#include <string>
	#include <sstream>
	#include <stdexcept>

	namespace boost {
		// Simple lexical_cast implementation using std::stringstream
		template<typename Target, typename Source>
		Target lexical_cast(const Source& arg) {
			std::stringstream ss;
			Target result;

			if (!(ss << arg) || !(ss >> result) || !(ss >> std::ws).eof()) {
				throw std::bad_cast();
			}

			return result;
		}

		// Specialization for converting to string (most common case)
		template<typename Source>
		std::string lexical_cast(const Source& arg) {
			return std::to_string(arg);
		}

		// Specialization for string to string (no-op)
		template<>
		inline std::string lexical_cast<std::string, std::string>(const std::string& arg) {
			return arg;
		}
	}
#else
	// Desktop build: Use actual Boost
	#include <boost/lexical_cast.hpp>
#endif

#endif // BOE_COMPAT_BOOST_LEXICAL_CAST_HPP
