/*
 * simple_types.hpp
 * BoE - Simplified types for web testing (Phase 3)
 *
 * Minimal rectangle/location types without SFML dependencies
 */

#ifndef BOE_SIMPLE_TYPES_HPP
#define BOE_SIMPLE_TYPES_HPP

// Simple rectangle structure
struct rectangle {
    int left, top, right, bottom;

    rectangle() : left(0), top(0), right(0), bottom(0) {}
    rectangle(int l, int t, int r, int b) : left(l), top(t), right(r), bottom(b) {}

    int width() const { return right - left; }
    int height() const { return bottom - top; }
};

// Simple location structure
struct location {
    int x, y;

    location() : x(0), y(0) {}
    location(int x, int y) : x(x), y(y) {}
};

#endif // BOE_SIMPLE_TYPES_HPP
