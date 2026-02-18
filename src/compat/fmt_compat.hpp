/*
 * fmt_compat.hpp
 * fmt::format compatibility layer for WebAssembly builds.
 * Provides a minimal fmt namespace using variadic templates and std::to_string.
 * For desktop builds, includes the real fmt library.
 */

#pragma once

#ifndef __EMSCRIPTEN__
	#include <fmt/format.h>
	#include <fmt/ranges.h>
	#include <fmt/compile.h>
#else
	#include <string>

	#define FMT_COMPILE(s) (s)
	#define BOOST_FALLTHROUGH [[fallthrough]]

	namespace fmt {
		inline std::string to_str(const std::string& s) { return s; }
		inline std::string to_str(const char* s) { return std::string(s); }
		template<typename T>
		inline std::string to_str(T val) { return std::to_string(val); }

		inline std::string format(const std::string& fmt_str) {
			return fmt_str;
		}

		template<typename T, typename... Args>
		std::string format(const std::string& fmt_str, T first, Args... rest) {
			size_t pos = fmt_str.find("{}");
			if (pos == std::string::npos) {
				return fmt_str;
			}
			std::string result = fmt_str.substr(0, pos) + to_str(first) + fmt_str.substr(pos + 2);
			return format(result, rest...);
		}

		// Stub for named arguments - just ignores the name
		template<typename T>
		struct arg_wrapper { T value; };
		template<typename T>
		inline arg_wrapper<T> arg(const char* name, T value) {
			return arg_wrapper<T>{value};
		}
		// Overload format to handle arg_wrapper
		template<typename T, typename... Args>
		std::string format(const std::string& fmt_str, arg_wrapper<T> first, Args... rest) {
			size_t pos = fmt_str.find("{}");
			if (pos == std::string::npos) {
				return fmt_str;
			}
			std::string result = fmt_str.substr(0, pos) + to_str(first.value) + fmt_str.substr(pos + 2);
			return format(result, rest...);
		}

		// Stub for fmt::join - joins a container with a separator
		template<typename Container>
		std::string join(const Container& container, const std::string& separator) {
			std::string result;
			bool first = true;
			for (const auto& item : container) {
				if (!first) {
					result += separator;
				}
				result += to_str(item);
				first = false;
			}
			return result;
		}
	}
#endif
