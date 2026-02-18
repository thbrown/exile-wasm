/*
 * boost_compat.hpp
 * Boost compatibility layer for WebAssembly builds
 * Provides minimal boost-like interfaces using C++17 standard library
 */

#pragma once

// Macro for marking unreachable code while satisfying the compiler's return requirement.
// Equivalent to Boost's BOOST_UNREACHABLE_RETURN.
#ifndef BOOST_UNREACHABLE_RETURN
	#ifdef __GNUC__
		#define BOOST_UNREACHABLE_RETURN(x) __builtin_unreachable()
	#else
		#define BOOST_UNREACHABLE_RETURN(x) return (x)
	#endif
#endif

#ifdef __EMSCRIPTEN__

#include <optional>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <type_traits>

namespace boost {
	// Optional type
	using std::optional;
	using std::nullopt;
	constexpr auto none = std::nullopt;  // boost::none equivalent

	// Lexical cast
	class bad_lexical_cast : public std::runtime_error {
	public:
		bad_lexical_cast() : std::runtime_error("bad lexical cast") {}
	};

	template<typename T, typename S>
	T lexical_cast(const S& arg) {
		if constexpr (std::is_same_v<T, std::string>) {
			if constexpr (std::is_same_v<S, std::string>) {
				return arg;
			} else if constexpr (std::is_arithmetic_v<S>) {
				return std::to_string(arg);
			} else {
				std::ostringstream oss;
				oss << arg;
				return oss.str();
			}
		} else if constexpr (std::is_integral_v<T>) {
			if constexpr (std::is_same_v<S, std::string>) {
				try {
					if constexpr (std::is_same_v<T, long long> || std::is_same_v<T, long>) {
						return static_cast<T>(std::stoll(arg));
					} else if constexpr (std::is_unsigned_v<T>) {
						return static_cast<T>(std::stoull(arg));
					} else {
						return static_cast<T>(std::stoi(arg));
					}
				} catch (...) {
					throw bad_lexical_cast();
				}
			} else if constexpr (std::is_arithmetic_v<S>) {
				return static_cast<T>(arg);
			}
		} else if constexpr (std::is_floating_point_v<T>) {
			if constexpr (std::is_same_v<S, std::string>) {
				try {
					if constexpr (std::is_same_v<T, float>) {
						return std::stof(arg);
					} else {
						return static_cast<T>(std::stod(arg));
					}
				} catch (...) {
					throw bad_lexical_cast();
				}
			} else if constexpr (std::is_arithmetic_v<S>) {
				return static_cast<T>(arg);
			}
		} else if constexpr (std::is_same_v<S, std::string>) {
			// Try stringstream as fallback
			std::istringstream iss(arg);
			T result;
			if (!(iss >> result)) {
				throw bad_lexical_cast();
			}
			return result;
		}
		throw bad_lexical_cast();
	}

	// String algorithm functions
	namespace algorithm {
		inline void trim(std::string& str) {
			auto start = str.find_first_not_of(" \t\n\r");
			auto end = str.find_last_not_of(" \t\n\r");
			if (start == std::string::npos) {
				str.clear();
			} else {
				str = str.substr(start, end - start + 1);
			}
		}

		inline void trim_right(std::string& str) {
			auto end = str.find_last_not_of(" \t\n\r");
			if (end == std::string::npos) {
				str.clear();
			} else {
				str = str.substr(0, end + 1);
			}
		}

		inline void trim_left(std::string& str) {
			auto start = str.find_first_not_of(" \t\n\r");
			if (start == std::string::npos) {
				str.clear();
			} else {
				str = str.substr(start);
			}
		}

		inline void to_lower(std::string& str) {
			for (char& c : str) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
		}

		inline bool starts_with(const std::string& str, const std::string& prefix) {
			return str.size() >= prefix.size() && str.substr(0, prefix.size()) == prefix;
		}

		inline bool ends_with(const std::string& str, const std::string& suffix) {
			return str.size() >= suffix.size() && str.substr(str.size() - suffix.size()) == suffix;
		}

		inline void erase_all(std::string& input, const std::string& search) {
			size_t pos = 0;
			while ((pos = input.find(search, pos)) != std::string::npos) {
				input.erase(pos, search.length());
			}
		}

		template<typename T>
		std::string erase_all_copy(std::string input, const T& search) {
			erase_all(input, search);
			return input;
		}

		inline void replace_all(std::string& input, const std::string& search, const std::string& replace) {
			size_t pos = 0;
			while ((pos = input.find(search, pos)) != std::string::npos) {
				input.replace(pos, search.length(), replace);
				pos += replace.length();
			}
		}

		inline std::string replace_all_copy(std::string input, const std::string& search, const std::string& replace) {
			replace_all(input, search, replace);
			return input;
		}

		inline void replace_first(std::string& input, const std::string& search, const std::string& replace) {
			size_t pos = input.find(search);
			if (pos != std::string::npos) {
				input.replace(pos, search.length(), replace);
			}
		}

		inline std::string replace_first_copy(std::string input, const std::string& search, const std::string& replace) {
			replace_first(input, search, replace);
			return input;
		}
	}

	// Expose commonly used algorithm functions at boost:: level
	// (some code calls boost::replace_first instead of boost::algorithm::replace_first)
	using algorithm::trim;
	using algorithm::trim_right;
	using algorithm::trim_left;
	using algorithm::to_lower;
	using algorithm::replace_all;
	using algorithm::replace_first;
	using algorithm::erase_all;

	// Note: boost::any is aliased to std::any in control.hpp for web builds
	// std::any doesn't have empty(), it has has_value() instead
}

#endif // __EMSCRIPTEN__
