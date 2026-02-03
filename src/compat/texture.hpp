/*
 * texture.hpp
 * Compatibility layer for SFML Texture and Image
 *
 * For WebAssembly builds, provides minimal texture interface
 * For desktop builds, uses SFML Texture/Image
 */

#ifndef BOE_COMPAT_TEXTURE_HPP
#define BOE_COMPAT_TEXTURE_HPP

#ifdef __EMSCRIPTEN__
	// Web build: Minimal texture/image interface
	#include <string>
	#include <vector>
	#include "compat/input_stream.hpp"

	namespace sf {
		// Color class (matches SFML API)
		struct Color {
			uint8_t r, g, b, a;

			Color() : r(0), g(0), b(0), a(255) {}
			Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255)
				: r(red), g(green), b(blue), a(alpha) {}

			bool operator==(const Color& other) const {
				return r == other.r && g == other.g && b == other.b && a == other.a;
			}
		};

		// Image class - holds pixel data
		class Image {
		private:
			unsigned int width_;
			unsigned int height_;
			std::vector<uint8_t> pixels_; // RGBA format

		public:
			Image() : width_(0), height_(0) {}

			bool create(unsigned int width, unsigned int height, const Color& color = Color(0, 0, 0)) {
				width_ = width;
				height_ = height;
				pixels_.resize(width * height * 4);
				// Fill with color
				for (size_t i = 0; i < width * height; ++i) {
					pixels_[i * 4 + 0] = color.r;
					pixels_[i * 4 + 1] = color.g;
					pixels_[i * 4 + 2] = color.b;
					pixels_[i * 4 + 3] = color.a;
				}
				return true;
			}

			bool create(unsigned int width, unsigned int height, const uint8_t* pixels) {
				width_ = width;
				height_ = height;
				pixels_.assign(pixels, pixels + (width * height * 4));
				return true;
			}

			bool loadFromFile(const std::string& filename) {
				// TODO: Implement using Emscripten VFS + image loading
				// For now, just create empty image
				return create(1, 1);
			}

			bool loadFromStream(InputStream& stream) {
				// TODO: Implement stream loading
				return create(1, 1);
			}

			bool loadFromMemory(const void* data, size_t size) {
				// TODO: Implement memory loading
				return create(1, 1);
			}

			unsigned int getSize() const { return width_ * height_ * 4; }
			unsigned int getWidth() const { return width_; }
			unsigned int getHeight() const { return height_; }
			const uint8_t* getPixelsPtr() const { return pixels_.data(); }

			void setPixel(unsigned int x, unsigned int y, const Color& color) {
				if (x < width_ && y < height_) {
					size_t idx = (y * width_ + x) * 4;
					pixels_[idx + 0] = color.r;
					pixels_[idx + 1] = color.g;
					pixels_[idx + 2] = color.b;
					pixels_[idx + 3] = color.a;
				}
			}

			Color getPixel(unsigned int x, unsigned int y) const {
				if (x < width_ && y < height_) {
					size_t idx = (y * width_ + x) * 4;
					return Color(pixels_[idx], pixels_[idx + 1], pixels_[idx + 2], pixels_[idx + 3]);
				}
				return Color();
			}
		};

		// Texture class - GPU-side image
		class Texture {
		private:
			unsigned int width_;
			unsigned int height_;
			// For web, we'll store a reference or ID to the actual Canvas texture
			// This is a minimal stub for now

		public:
			Texture() : width_(0), height_(0) {}

			bool create(unsigned int width, unsigned int height) {
				width_ = width;
				height_ = height;
				return true;
			}

			bool loadFromFile(const std::string& filename) {
				// TODO: Implement using Canvas texture loading
				return create(1, 1);
			}

			bool loadFromStream(InputStream& stream) {
				return create(1, 1);
			}

			bool loadFromImage(const Image& image) {
				return create(image.getWidth(), image.getHeight());
			}

			bool loadFromMemory(const void* data, size_t size) {
				return create(1, 1);
			}

			unsigned int getSize() const { return width_ * height_ * 4; }
			unsigned int getWidth() const { return width_; }
			unsigned int getHeight() const { return height_; }

			Image copyToImage() const {
				Image img;
				img.create(width_, height_);
				return img;
			}
		};
	}

#else
	// Desktop build: Use actual SFML
	#include <SFML/Graphics/Texture.hpp>
	#include <SFML/Graphics/Image.hpp>
	#include <SFML/Graphics/Color.hpp>
#endif

#endif // BOE_COMPAT_TEXTURE_HPP
