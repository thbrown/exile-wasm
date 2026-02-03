/*
 * event.hpp
 * Compatibility layer for SFML Event
 *
 * For WebAssembly builds, provides minimal event stubs
 * For desktop builds, uses SFML Event
 */

#ifndef BOE_COMPAT_EVENT_HPP
#define BOE_COMPAT_EVENT_HPP

#ifdef __EMSCRIPTEN__
	// Web build: Minimal event stubs
	namespace sf {
		// Keyboard
		namespace Keyboard {
			enum Key {
				Unknown = -1,
				A = 0, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
				Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
				Escape, LControl, LShift, LAlt, LSystem,
				RControl, RShift, RAlt, RSystem,
				Menu, LBracket, RBracket, Semicolon, Comma, Period, Quote, Slash, Backslash,
				Tilde, Equal, Hyphen, Space, Enter, Backspace, Tab,
				PageUp, PageDown, End, Home, Insert, Delete,
				Add, Subtract, Multiply, Divide,
				Left, Right, Up, Down,
				Numpad0, Numpad1, Numpad2, Numpad3, Numpad4,
				Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
				F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15,
				Pause,
				KeyCount
			};
		}

		// Mouse
		namespace Mouse {
			enum Button {
				Left,
				Right,
				Middle,
				XButton1,
				XButton2,
				ButtonCount
			};
		}

		// Event structure
		struct Event {
			enum EventType {
				Closed,
				Resized,
				LostFocus,
				GainedFocus,
				TextEntered,
				KeyPressed,
				KeyReleased,
				MouseWheelScrolled,
				MouseButtonPressed,
				MouseButtonReleased,
				MouseMoved,
				MouseEntered,
				MouseLeft,
				Count
			};

			struct SizeEvent {
				unsigned int width, height;
			};

			struct KeyEvent {
				Keyboard::Key code;
				bool alt, control, shift, system;
			};

			struct TextEvent {
				uint32_t unicode;
			};

			struct MouseMoveEvent {
				int x, y;
			};

			struct MouseButtonEvent {
				Mouse::Button button;
				int x, y;
			};

			struct MouseWheelScrollEvent {
				float delta;
				int x, y;
			};

			EventType type;

			union {
				SizeEvent size;
				KeyEvent key;
				TextEvent text;
				MouseMoveEvent mouseMove;
				MouseButtonEvent mouseButton;
				MouseWheelScrollEvent mouseWheelScroll;
			};
		};
	}

#else
	// Desktop build: Use actual SFML
	#include <SFML/Window/Event.hpp>
	#include <SFML/Window/Keyboard.hpp>
	#include <SFML/Window/Mouse.hpp>
#endif

#endif // BOE_COMPAT_EVENT_HPP
