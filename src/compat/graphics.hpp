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
	#include <emscripten.h>
	#include "compat/texture.hpp"
	#include "compat/event.hpp"
	#include "compat/draw_dedup.hpp"
	#include <string>
	#include <vector>
	#include <iostream>

	// Forward declare location for RenderWindow::mapPixelToCoords overload
	struct location;

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

		// Vector2 operators
		template<typename T>
		Vector2<T> operator-(const Vector2<T>& left, const Vector2<T>& right) {
			return Vector2<T>(left.x - right.x, left.y - right.y);
		}

		template<typename T>
		Vector2<T> operator+(const Vector2<T>& left, const Vector2<T>& right) {
			return Vector2<T>(left.x + right.x, left.y + right.y);
		}

		template<typename T>
		Vector2<T>& operator+=(Vector2<T>& left, const Vector2<T>& right) {
			left.x += right.x;
			left.y += right.y;
			return left;
		}

		template<typename T>
		Vector2<T>& operator*=(Vector2<T>& left, T scalar) {
			left.x *= scalar;
			left.y *= scalar;
			return left;
		}

		template<typename T>
		Vector2<T> operator*(const Vector2<T>& left, T scalar) {
			return Vector2<T>(left.x * scalar, left.y * scalar);
		}

		// Implement Mouse::getPosition() now that Vector2 is defined
		inline Vector2i Mouse::getPosition(const Window& relativeTo) {
			// Stub - in real web implementation would get from browser
			return Vector2i(0, 0);
		}

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
			void setPosition(float x, float y) { position_ = Vector2f(x, y); }
			void move(const Vector2f& offset) { position_ += offset; }
			void move(float x, float y) { position_ += Vector2f(x, y); }
			void setScale(const Vector2f& scale) { scale_ = scale; }
			void setScale(float x, float y) { scale_ = Vector2f(x, y); }
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
			Sprite(const Texture& texture, const IntRect& rect)
				: texture_(&texture), textureRect_(rect) {}
			template<typename R>
			Sprite(const Texture& texture, const R& rect)
				: texture_(&texture), textureRect_(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top) {}

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

		// Glyph struct for Font::getGlyph
		struct Glyph {
			float advance = 10.0f;
			FloatRect bounds;
			IntRect textureRect;
		};

		// String type for SFML compatibility
		using String = std::string;

		// Font class (stub)
		class Font {
		public:
			bool loadFromFile(const std::string& filename) { return true; }
			const Texture& getTexture(unsigned int characterSize) const {
				static Texture tex;
				return tex;
			}
			Glyph getGlyph(unsigned int codePoint, unsigned int characterSize, bool bold) const {
				return Glyph();
			}
			float getKerning(unsigned int first, unsigned int second, unsigned int characterSize) const {
				return 0.0f;
			}
		};

		// Text class (stub)
		class Text : public Drawable, public Transformable {
		public:
			enum Style {
				Regular = 0,
				Bold = 1 << 0,
				Italic = 1 << 1,
				Underlined = 1 << 2
			};

		private:
			std::string string_;
			const Font* font_ = nullptr;
			unsigned int characterSize_ = 30;
			Color fillColor_{255, 255, 255, 255};
			float lineSpacing_ = 1.0f;
			int style_ = Regular;
		public:
			Text() {}
			Text(const std::string& str, const Font& font, unsigned int size = 30)
				: string_(str), font_(&font), characterSize_(size) {}

			void setString(const std::string& str) { string_ = str; }
			void setFont(const Font& font) { font_ = &font; }
			void setCharacterSize(unsigned int size) { characterSize_ = size; }
			void setFillColor(const Color& color) { fillColor_ = color; }
			void setLineSpacing(float spacing) { lineSpacing_ = spacing; }
			void setStyle(int style) { style_ = style; }

			const std::string& getString() const { return string_; }
			const Font* getFont() const { return font_; }
			unsigned int getCharacterSize() const { return characterSize_; }
			const Color& getFillColor() const { return fillColor_; }
			float getLineSpacing() const { return lineSpacing_; }
			int getStyle() const { return style_; }

			FloatRect getLocalBounds() const {
				// Rough estimate: 10 pixels per character
				return FloatRect(0, 0, static_cast<float>(string_.length() * characterSize_ * 0.6f), static_cast<float>(characterSize_));
			}
			FloatRect getGlobalBounds() const {
				FloatRect local = getLocalBounds();
				auto pos = getPosition();
				return FloatRect(pos.x + local.left, pos.y + local.top, local.width, local.height);
			}
		};

		// Vertex for VertexArray
		struct Vertex {
			Vector2f position;
			Color color;
			Vector2f texCoords;
			Vertex() {}
			Vertex(const Vector2f& pos, const Color& col = Color(255, 255, 255))
				: position(pos), color(col) {}
		};

		// Primitive type for VertexArray
		enum PrimitiveType {
			Points,
			Lines,
			LineStrip,
			Triangles,
			TriangleStrip,
			TriangleFan,
			Quads,
			LinesStrip = LineStrip  // Alias
		};

		// VertexArray (stub)
		class VertexArray : public Drawable {
		private:
			std::vector<Vertex> vertices_;
			PrimitiveType type_ = Points;
		public:
			VertexArray() {}
			VertexArray(PrimitiveType type, size_t count = 0)
				: type_(type), vertices_(count) {}

			size_t getVertexCount() const { return vertices_.size(); }
			Vertex& operator[](size_t index) { return vertices_[index]; }
			const Vertex& operator[](size_t index) const { return vertices_[index]; }
			void append(const Vertex& vertex) { vertices_.push_back(vertex); }
			void resize(size_t count) { vertices_.resize(count); }
		};

		// Forward declare rectangle for getGlobalBounds
		struct rectangle;

		// Shape base class (stub)
		class Shape : public Drawable, public Transformable {
		protected:
			Color fillColor_{255, 255, 255, 255};
			Color outlineColor_{255, 255, 255, 255};
			float outlineThickness_ = 0;
		public:
			virtual ~Shape() {}
			virtual size_t getPointCount() const { return 0; }
			virtual Vector2f getPoint(size_t index) const { return Vector2f(); }
			void update() {}  // Stub for shape update
			FloatRect getGlobalBounds() const { return FloatRect(0, 0, 100, 100); }  // Stub

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

		// Forward declare Texture for Shader
		class Texture;

		// Shader (stub) - GLSL shaders not supported in Canvas 2D, so this is a no-op
		class Shader {
		public:
			enum Type {
				Vertex,
				Fragment
			};

			// CurrentTexture is used as a special value for texture uniforms
			struct CurrentTextureType {};
			static const CurrentTextureType CurrentTexture;

			Shader() {}

			// Stub setUniform functions - do nothing in Canvas 2D
			template<typename T>
			void setUniform(const std::string& name, const T& value) {}

			void setUniform(const std::string& name, const Texture& texture) {}
			void setUniform(const std::string& name, CurrentTextureType) {}

			bool loadFromFile(const std::string& filename, Type type) { return true; }
			bool loadFromMemory(const std::string& shader, Type type) { return true; }
			bool loadFromMemory(const std::string& vertexShader, const std::string& fragmentShader) { return true; }
		};

		// Define the CurrentTexture constant
		inline const Shader::CurrentTextureType Shader::CurrentTexture = Shader::CurrentTextureType();

		// BlendMode (stub)
		enum BlendMode {
			BlendNone = 0,
			BlendAlpha,
			BlendAdd,
			BlendMultiply
		};

		// VideoMode (stub)
		class VideoMode {
		public:
			unsigned int width, height, bitsPerPixel;
			VideoMode() : width(0), height(0), bitsPerPixel(32) {}
			VideoMode(unsigned int w, unsigned int h, unsigned int bpp = 32)
				: width(w), height(h), bitsPerPixel(bpp) {}

			static VideoMode getDesktopMode() {
				return VideoMode(1024, 768, 32); // Default resolution for web
			}
		};

		// ContextSettings (stub)
		struct ContextSettings {
			unsigned int depthBits = 0;
			unsigned int stencilBits = 0;
			unsigned int antialiasingLevel = 0;
			unsigned int majorVersion = 1;
			unsigned int minorVersion = 1;

			ContextSettings(unsigned int depth = 0, unsigned int stencil = 0, unsigned int antialiasing = 0)
				: depthBits(depth), stencilBits(stencil), antialiasingLevel(antialiasing) {}
		};

		// Style constants (stub)
		namespace Style {
			enum {
				None = 0,
				Titlebar = 1 << 0,
				Resize = 1 << 1,
				Close = 1 << 2,
				Fullscreen = 1 << 3,
				Default = Titlebar | Resize | Close
			};
		}

		// Window base class (stub)
		class Window {
		protected:
			unsigned int width_ = 800;
			unsigned int height_ = 600;
			std::string title_;
			bool open_ = true;
			bool visible_ = true;
			Vector2i position_{0, 0};
		public:
			Window() {}
			Window(unsigned int width, unsigned int height, const std::string& title)
				: width_(width), height_(height), title_(title) {}
			virtual ~Window() {}

			void create(const VideoMode& mode, const std::string& title, unsigned int style = Style::Default, const ContextSettings& settings = ContextSettings()) {
				width_ = mode.width;
				height_ = mode.height;
				title_ = title;
				open_ = true;
			}

			bool isOpen() const { return open_; }
			void close() { open_ = false; }

			Vector2u getSize() const { return Vector2u(width_, height_); }
			void setSize(const Vector2u& size) { width_ = size.x; height_ = size.y; }

			void setPosition(const Vector2i& pos) { position_ = pos; }
			Vector2i getPosition() const { return position_; }

			void setVisible(bool visible) { visible_ = visible; }
			bool isVisible() const { return visible_; }

			bool pollEvent(Event& event) { return false; }
			void setTitle(const std::string& title) { title_ = title; }
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
			void setCenter(float x, float y) { center_ = Vector2f(x, y); }
			void setSize(const Vector2f& size) { size_ = size; }
			void setSize(float w, float h) { size_ = Vector2f(w, h); }
			void setViewport(const FloatRect& viewport) { viewport_ = viewport; }

			void reset(const FloatRect& rect) {
				size_ = Vector2f(rect.width, rect.height);
				center_ = Vector2f(rect.left + rect.width / 2.0f, rect.top + rect.height / 2.0f);
			}

			const Vector2f& getCenter() const { return center_; }
			const Vector2f& getSize() const { return size_; }
			const FloatRect& getViewport() const { return viewport_; }
		};

		// RenderStates (stub)
		struct RenderStates {
			const Texture* texture = nullptr;
			const Shader* shader = nullptr;
			BlendMode blendMode = BlendAlpha;
			RenderStates() {}
			RenderStates(const Texture* tex) : texture(tex) {}
			RenderStates(const Shader* shd) : shader(shd) {}
			RenderStates(BlendMode mode) : blendMode(mode) {}
		};

		// RenderTarget base class (stub)
		class RenderTarget {
		public:
			virtual ~RenderTarget() {}
			virtual void clear(const Color& color = Color(0, 0, 0)) = 0;
			virtual void draw(const Drawable& drawable, const RenderStates& states = RenderStates()) = 0;
			virtual Vector2u getSize() const = 0;
			virtual void setView(const View& view) = 0;
			virtual const View& getView() const = 0;
			virtual View getDefaultView() const = 0;

			// Coordinate transformation methods (stubs)
			template<typename T>
			Vector2i mapCoordsToPixel(const Vector2<T>& point) const {
				return Vector2i(static_cast<int>(point.x), static_cast<int>(point.y));
			}
			template<typename T>
			Vector2i mapCoordsToPixel(const Vector2<T>& point, const View& view) const {
				return Vector2i(static_cast<int>(point.x), static_cast<int>(point.y));
			}
			// Overload for location (defined after location.hpp is included)
			Vector2i mapCoordsToPixel(const ::location& point) const;
			Vector2i mapCoordsToPixel(const ::location& point, const View& view) const;
		};

		// RenderTexture (stub) - off-screen rendering target
		class RenderTexture : public RenderTarget {
		private:
			unsigned int width_ = 0;
			unsigned int height_ = 0;
			Texture texture_;
		public:
			RenderTexture() {}

			bool create(unsigned int width, unsigned int height) {
				width_ = width;
				height_ = height;
				texture_.create(width, height);
				return true;
			}

			void clear(const Color& color = Color(0, 0, 0)) override {}
			void display() {}

			void draw(const Drawable& drawable, const RenderStates& states = RenderStates()) override {}

			Vector2u getSize() const override { return Vector2u(width_, height_); }
			void setView(const View& view) override {}
			const View& getView() const override { static View v; return v; }
			View getDefaultView() const override { return View(); }

			const Texture& getTexture() const { return texture_; }
			bool setActive(bool active = true) { return true; }
		};

		// RenderWindow (stub) - main rendering window
		class RenderWindow : public Window, public RenderTarget {
		private:
			Color clearColor_{0, 0, 0};
		public:
			RenderWindow() {}
			RenderWindow(unsigned int width, unsigned int height, const std::string& title)
				: Window(width, height, title) {}

			Vector2u getSize() const override { return Vector2u(width_, height_); }

			void clear(const Color& color = Color(0, 0, 0)) override {
				clearColor_ = color;
				#ifdef __EMSCRIPTEN__
				static int clear_count = 0;
				static int last_log = 0;
				static bool size_set = false;
				clear_count++;

				// Set canvas size once to ensure all sprites are visible
				if(!size_set) {
					EM_ASM({
						var canvas = Module.canvas;
						var oldW = canvas.width;
						var oldH = canvas.height;
						canvas.width = 800;
						canvas.height = 600;
						console.log('Canvas resized from ' + oldW + 'x' + oldH + ' to 800x600');
					});
					size_set = true;
				}

				if(clear_count - last_log >= 60) { // Log once per second
					std::cout << "clear() called " << clear_count << " times, color=("
					          << (int)color.r << "," << (int)color.g << "," << (int)color.b << ")" << std::endl;
					last_log = clear_count;
				}
				draw_dedup::newFrame(); // Reset draw tracking for new frame
				EM_ASM({
					var canvas = Module.canvas;
					if(canvas && canvas.getContext) {
						var ctx = canvas.getContext('2d');
						ctx.fillStyle = 'rgb(' + $0 + ',' + $1 + ',' + $2 + ')';
						ctx.fillRect(0, 0, canvas.width, canvas.height);
					}
				}, color.r, color.g, color.b);
				#endif
			}

			void display() {
				#ifdef __EMSCRIPTEN__
				// For Canvas 2D, display() doesn't need to do anything
				// Drawing commands are immediately visible
				#endif
			}

			void draw(const Drawable& drawable, const RenderStates& states = RenderStates()) override {
				#ifdef __EMSCRIPTEN__
				const Sprite* sprite = dynamic_cast<const Sprite*>(&drawable);
				if(sprite) {
					auto pos = sprite->getPosition();
					auto texRect = sprite->getTextureRect();
					auto scale = sprite->getScale();
					auto color = sprite->getColor();
					const Texture* tex = sprite->getTexture();

					float w = texRect.width * scale.x;
					float h = texRect.height * scale.y;

					// Deduplication disabled - let all sprites through
					// Log every sprite draw for debugging
					static int total_draws = 0;
					if(total_draws < 20) {
						std::cout << "DRAW #" << total_draws << ": pos=(" << pos.x << "," << pos.y << ") "
						          << "texRect=(" << texRect.left << "," << texRect.top << ","
						          << texRect.width << "," << texRect.height << ") "
						          << "scale=(" << scale.x << "," << scale.y << ") "
						          << "dest=(" << w << "x" << h << ")";
						if(tex) std::cout << " tex=" << tex->getFilename();
						std::cout << std::endl;
					}
					total_draws++;

					// Draw using texture if available, otherwise colored rect
					if(tex && !tex->getFilename().empty()) {
						EM_ASM_({
							var ctx = Module.canvas.getContext('2d');
							var filename = UTF8ToString($0);

							// Load texture if not cached
							if (!Module.textureCache) {
								Module.textureCache = {};
							}

							if (!Module.textureCache[filename]) {
								// Try to load from VFS
								try {
									var data = FS.readFile(filename);
									var blob = new Blob([data]);
									var url = URL.createObjectURL(blob);
									var img = new Image();
									img.src = url;
									Module.textureCache[filename] = img;
									console.log('Loaded texture on-demand: ' + filename);
								} catch(e) {
									console.error('Failed to load texture: ' + filename + ' - ' + e);
									// Draw red error rect
									ctx.fillStyle = 'red';
									ctx.fillRect($1, $2, $7, $8);
									return;
								}
							}

							var img = Module.textureCache[filename];
							if (img.complete && img.naturalWidth > 0) {
								// Use texture rect exactly as specified - game code has correct pixel coordinates
								ctx.drawImage(img, $3, $4, $5, $6, $1, $2, $7, $8);
							} else {
								ctx.fillStyle = 'yellow';
								ctx.fillRect($1, $2, $7, $8);
								ctx.strokeStyle = 'black';
								ctx.strokeRect($1, $2, $7, $8);
							}
						}, tex->getFilename().c_str(), pos.x, pos.y,
						   texRect.left, texRect.top, texRect.width, texRect.height,
						   w, h);
					} else {
						// No texture - draw colored rect for debugging
						EM_ASM_({
							var ctx = Module.canvas.getContext('2d');
							ctx.fillStyle = 'magenta';
							ctx.fillRect($0, $1, $2, $3);
							ctx.strokeStyle = 'white';
							ctx.lineWidth = 1;
							ctx.strokeRect($0, $1, $2, $3);
						}, pos.x, pos.y, w, h);
					}
					return;
				}

				// Handle Text rendering
				const Text* text = dynamic_cast<const Text*>(&drawable);
				if(text) {
					auto pos = text->getPosition();
					auto str = text->getString();
					auto size = text->getCharacterSize();
					auto color = text->getFillColor();

					if(str.empty()) return;

					EM_ASM_({
						var ctx = Module.canvas.getContext('2d');
						var text = UTF8ToString($0);
						var x = $1;
						var y = $2;
						var size = $3;
						var r = $4;
						var g = $5;
						var b = $6;
						var a = $7;

						ctx.font = size + 'px sans-serif';
						ctx.fillStyle = 'rgba(' + r + ',' + g + ',' + b + ',' + (a/255) + ')';
						ctx.textBaseline = 'top';
						ctx.fillText(text, x, y);
					}, str.c_str(), pos.x, pos.y, size, color.r, color.g, color.b, color.a);
					return;
				}
				#endif
			}

			void setView(const View& view) override {}
			const View& getView() const override { static View v; return v; }
			View getDefaultView() const override { return View(); }

			bool setActive(bool active = true) { return true; }
			void setIcon(unsigned int width, unsigned int height, const uint8_t* pixels) {
				// Stub - would set window icon
			}
			void setKeyRepeatEnabled(bool enabled) {
				// Stub - in real implementation would control key repeat
			}

			// Map pixel coordinates to world coordinates
			template<typename T>
			Vector2f mapPixelToCoords(const Vector2<T>& point, const View& view) const {
				// Stub - in real implementation would transform using view
				return Vector2f(static_cast<float>(point.x), static_cast<float>(point.y));
			}

			// Overload with default view
			template<typename T>
			Vector2f mapPixelToCoords(const Vector2<T>& point) const {
				return mapPixelToCoords(point, getDefaultView());
			}

			// Overload for location (defined after location.hpp is included)
			Vector2f mapPixelToCoords(const ::location& point, const View& view) const;
			Vector2f mapPixelToCoords(const ::location& point) const;
		};
	}

	// Note: The implementation of RenderWindow::mapPixelToCoords() for location
	// is provided in location.hpp after the location struct is fully defined

	// OpenGL stub functions (for code that directly calls OpenGL)
	#define GL_LINE_WIDTH 0x0B21
	#define GL_SCISSOR_TEST 0x0C11
	#define GL_STENCIL_TEST 0x0B90
	#define GL_STENCIL_BUFFER_BIT 0x00000400
	#define GL_ALWAYS 0x0207
	#define GL_EQUAL 0x0202
	#define GL_REPLACE 0x1E01
	#define GL_KEEP 0x1E00
	#define GL_ZERO 0

	inline void glGetFloatv(unsigned int pname, float* params) { if(params) *params = 1.0f; }
	inline void glLineWidth(float width) {}
	inline void glEnable(unsigned int cap) {}
	inline void glDisable(unsigned int cap) {}
	inline void glScissor(int x, int y, int width, int height) {}
	inline void glClearStencil(int s) {}
	inline void glClear(unsigned int mask) {}
	inline void glStencilFunc(unsigned int func, int ref, unsigned int mask) {}
	inline void glStencilOp(unsigned int fail, unsigned int zfail, unsigned int zpass) {}

#else
	// Desktop build: Use actual SFML
	#include <SFML/Graphics.hpp>
#endif

#endif // BOE_COMPAT_GRAPHICS_HPP
