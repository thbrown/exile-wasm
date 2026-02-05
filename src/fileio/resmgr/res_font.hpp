/*
 *  restypes.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 10-08-25.
 *
 */

#ifndef BOE_RES_FONT_HPP
#define BOE_RES_FONT_HPP

#include "resmgr.hpp"
#ifndef __EMSCRIPTEN__
	#include <SFML/Graphics.hpp>
#else
	#include "compat/graphics.hpp"
#endif

using FontRsrc = ResMgr::cPointer<sf::Font>;

namespace ResMgr {
	extern cPool<sf::Font> fonts;
}

#endif
