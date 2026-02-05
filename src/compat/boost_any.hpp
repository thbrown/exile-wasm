/*
 * boost_any.hpp
 * Compatibility shim for boost::any
 *
 * For WebAssembly builds, uses std::any (C++17)
 */

#ifndef BOE_COMPAT_BOOST_ANY_HPP
#define BOE_COMPAT_BOOST_ANY_HPP

#ifdef __EMSCRIPTEN__
	// Web build: Use std::any from C++17
	#include <any>

	namespace boost {
		using std::any;
		using std::any_cast;
		using std::bad_any_cast;
	}
#else
	// Desktop build: Use actual Boost
	#include <boost/any.hpp>
#endif

#endif // BOE_COMPAT_BOOST_ANY_HPP
