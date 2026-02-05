/*
 *  restypes.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 10-08-25.
 *
 */

#ifndef BOE_RES_IMAGE_HPP
#define BOE_RES_IMAGE_HPP

#ifndef __EMSCRIPTEN__
	#include <SFML/Graphics.hpp>
#else
	#include "compat/graphics.hpp"
#endif
#include "resmgr.hpp"

using ImageRsrc = ResMgr::cPointer<sf::Texture>;

namespace ResMgr {
	extern cPool<sf::Texture> graphics;
}

#endif
