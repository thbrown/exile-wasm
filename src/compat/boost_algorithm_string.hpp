/*
 * boost_algorithm_string.hpp
 * Compatibility shim for boost::algorithm::string
 *
 * For WebAssembly builds, provides std-based implementation
 */

#ifndef BOE_COMPAT_BOOST_ALGORITHM_STRING_HPP
#define BOE_COMPAT_BOOST_ALGORITHM_STRING_HPP

#ifdef __EMSCRIPTEN__
	// Web build: Implement boost::split using std::string
	#include <string>
	#include <vector>
	#include <algorithm>

	namespace boost {
		namespace algorithm {
			// Predicate for is_any_of
			class is_any_of_pred {
			private:
				std::string chars_;
			public:
				is_any_of_pred(const std::string& chars) : chars_(chars) {}

				bool operator()(char c) const {
					return chars_.find(c) != std::string::npos;
				}
			};

			// Helper to create is_any_of predicate
			inline is_any_of_pred is_any_of(const std::string& chars) {
				return is_any_of_pred(chars);
			}

			// Simple split implementation
			template<typename Container, typename Predicate>
			void split(Container& result, const std::string& str, Predicate pred) {
				result.clear();
				std::string current;

				for (char c : str) {
					if (pred(c)) {
						result.push_back(current);
						current.clear();
					} else {
						current += c;
					}
				}

				// Add the last token
				result.push_back(current);
			}
		}

		// Make split and is_any_of available in boost namespace for convenience
		using algorithm::split;
		using algorithm::is_any_of;
	}
#else
	// Desktop build: Use actual Boost
	#include <boost/algorithm/string.hpp>
#endif

#endif // BOE_COMPAT_BOOST_ALGORITHM_STRING_HPP
