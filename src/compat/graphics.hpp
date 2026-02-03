/*
 * graphics.hpp
 * Compatibility layer for SFML Graphics
 *
 * For WebAssembly builds, provides minimal graphics stubs
 * For desktop builds, uses SFML Graphics
 */

#ifndef BOE_COMPAT_GRAPHICS_HPP
#define BOE_COMPAT_GRAPHICS_HPP

#ifdef __EMSCRIPTEN__
	// Web build: Minimal graphics stubs
	#include "compat/texture.hpp"
	#include "location.hpp" // For rectangle
	#include <string>
	#include <vector>

	namespace sf {
		// Types from texture.hpp are already defined there
		// Just include them in this namespace
		// (They're defined in compat/texture.hpp in the sf namespace)

		// Basic geometric types
		template<typename T>
		struct Vector2 {
			T x, y;
			Vector2() : x(0), y(0) {}
			Vector2(T x, T y) : x(x), y(y) {}
		};

		typedef Vector2<int> Vector2i;
		typedef Vector2<unsigned int> Vector2u;
		typedef Vector2<float> Vector2f;

		// Rect type
		template<typename T>
		struct Rect {
			T left, top, width, height;
			Rect() : left(0), top(0), width(0), height(0) {}
			Rect(T left, T top, T width, T height)
				: left(left), top(top), width(width), height(height) {}
			bool contains(T x, T y) const {
				return x >= left && x < left + width && y >= top && y < top + height;
			}
		};

		typedef Rect<int> IntRect;
		typedef Rect<float> FloatRect;

		// Drawable interface
		class Drawable {
		public:
			virtual ~Drawable() {}
		};

		// Transformable interface
		class Transformable {
		private:
			Vector2f position_;
			Vector2f scale_{1, 1};
			float rotation_ = 0;
		public:
			void setPosition(const Vector2f& pos) { position_ = pos; }
			void setScale(const Vector2f& scale) { scale_ = scale; }
			void setRotation(float angle) { rotation_ = angle; }
			const Vector2f& getPosition() const { return position_; }
			const Vector2f& getScale() const { return scale_; }
			float getRotation() const { return rotation_; }
		};

		// Sprite class
		class Sprite : public Drawable, public Transformable {
		private:
			const Texture* texture_ = nullptr;
			IntRect textureRect_;
			Color color_{255, 255, 255, 255};
		public:
			Sprite() {}
			explicit Sprite(const Texture& texture) : texture_(&texture) {}

			void setTexture(const Texture& texture, bool resetRect = false) {
				texture_ = &texture;
				if (resetRect) {
					textureRect_ = IntRect(0, 0, texture.getWidth(), texture.getHeight());
				}
			}
			void setTextureRect(const IntRect& rect) { textureRect_ = rect; }
			void setColor(const Color& color) { color_ = color; }

			const Texture* getTexture() const { return texture_; }
			const IntRect& getTextureRect() const { return textureRect_; }
			const Color& getColor() const { return color_; }
		};

		// Font class (stub)
		class Font {
		public:
			bool loadFromFile(const std::string& filename) { return true; }
		};

		// Text class (stub)
		class Text : public Drawable, public Transformable {
		private:
			std::string string_;
			const Font* font_ = nullptr;
			unsigned int characterSize_ = 30;
			Color fillColor_{255, 255, 255, 255};
		public:
			Text() {}
			Text(const std::string& str, const Font& font, unsigned int size = 30)
				: string_(str), font_(&font), characterSize_(size) {}

			void setString(const std::string& str) { string_ = str; }
			void setFont(const Font& font) { font_ = &font; }
			void setCharacterSize(unsigned int size) { characterSize_ = size; }
			void setFillColor(const Color& color) { fillColor_ = color; }

			const std::string& getString() const { return string_; }
			const Font* getFont() const { return font_; }
			unsigned int getCharacterSize() const { return characterSize_; }
			const Color& getFillColor() const { return fillColor_; }
		};

		// Shape base class (stub)
		class Shape : public Drawable, public Transformable {
		protected:
			Color fillColor_{255, 255, 255, 255};
			Color outlineColor_{255, 255, 255, 255};
			float outlineThickness_ = 0;
		public:
			void setFillColor(const Color& color) { fillColor_ = color; }
			void setOutlineColor(const Color& color) { outlineColor_ = color; }
			void setOutlineThickness(float thickness) { outlineThickness_ = thickness; }

			const Color& getFillColor() const { return fillColor_; }
			const Color& getOutlineColor() const { return outlineColor_; }
			float getOutlineThickness() const { return outlineThickness_; }
		};

		// RectangleShape (stub)
		class RectangleShape : public Shape {
		private:
			Vector2f size_;
		public:
			explicit RectangleShape(const Vector2f& size = Vector2f(0, 0)) : size_(size) {}
			void setSize(const Vector2f& size) { size_ = size; }
			const Vector2f& getSize() const { return size_; }
		};

		// CircleShape (stub)
		class CircleShape : public Shape {
		private:
			float radius_ = 0;
		public:
			explicit CircleShape(float radius = 0) : radius_(radius) {}
			void setRadius(float radius) { radius_ = radius; }
			float getRadius() const { return radius_; }
		};

		// View (stub)
		class View {
		private:
			FloatRect viewport_{0, 0, 1, 1};
			Vector2f center_;
			Vector2f size_;
		public:
			View() {}
			explicit View(const FloatRect& rect) : viewport_(rect) {}

			void setCenter(const Vector2f& center) { center_ = center; }
			void setSize(const Vector2f& size) { size_ = size; }
			void setViewport(const FloatRect& viewport) { viewport_ = viewport; }

			const Vector2f& getCenter() const { return center_; }
			const Vector2f& getSize() const { return size_; }
			const FloatRect& getViewport() const { return viewport_; }
		};

		// RenderStates (stub)
		struct RenderStates {
			const Texture* texture = nullptr;
			RenderStates() {}
			RenderStates(const Texture* tex) : texture(tex) {}
		};

		// RenderWindow (stub) - main rendering window
		class RenderWindow {
		private:
			unsigned int width_ = 800;
			unsigned int height_ = 600;
			std::string title_;
			bool open_ = true;
		public:
			RenderWindow() {}
			RenderWindow(unsigned int width, unsigned int height, const std::string& title)
				: width_(width), height_(height), title_(title) {}

			bool isOpen() const { return open_; }
			void close() { open_ = false; }

			Vector2u getSize() const { return Vector2u(width_, height_); }
			void setSize(const Vector2u& size) { width_ = size.x; height_ = size.y; }

			void clear(const Color& color = Color(0, 0, 0)) {}
			void display() {}

			void draw(const Drawable& drawable, const RenderStates& states = RenderStates()) {}

			void setView(const View& view) {}
			const View& getView() const { static View v; return v; }
			View getDefaultView() const { return View(); }
		};
	}

#else
	// Desktop build: Use actual SFML
	#include <SFML/Graphics.hpp>
#endif

#endif // BOE_COMPAT_GRAPHICS_HPP
