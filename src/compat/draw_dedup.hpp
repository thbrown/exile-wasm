/* draw_dedup.hpp - Simple draw deduplication for web builds */
#ifndef DRAW_DEDUP_HPP
#define DRAW_DEDUP_HPP

#ifdef __EMSCRIPTEN__
#include <set>
#include <string>
#include <cstdio>
#include <iostream>

namespace draw_dedup {
	static std::set<std::string> drawnThisFrame;
	static int frame_num = 0;
	static int draws_this_frame = 0;

	inline void newFrame() {
		drawnThisFrame.clear();
		draws_this_frame = 0;
		frame_num++;
	}

	inline bool shouldDraw(float x, float y, float w, float h, int r, int g, int b) {
		char key[128];
		snprintf(key, sizeof(key), "%.0f,%.0f,%.0fx%.0f,%d,%d,%d", x, y, w, h, r, g, b);
		std::string sprite_key(key);

		if(drawnThisFrame.count(sprite_key) > 0) {
			return false; // Already drawn
		}
		drawnThisFrame.insert(sprite_key);
		draws_this_frame++;

		// Log first few draws each frame
		if(draws_this_frame <= 10) {
			std::cout << "  Draw #" << draws_this_frame << ": (" << x << "," << y << ") " << w << "x" << h << std::endl;
		}
		return true; // Draw it
	}
}
#endif

#endif
