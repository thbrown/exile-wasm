/*
 * tgui.hpp
 * Minimal TGUI stubs for WASM builds
 * Provides just enough to compile the menu system without visual menubar
 */

#ifndef BOE_COMPAT_TGUI_HPP
#define BOE_COMPAT_TGUI_HPP

#ifdef __EMSCRIPTEN__

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include "compat/event.hpp"

// Minimal TGUI namespace with stubbed types
namespace tgui {
	// Forward declarations
	class Widget;
	class MenuBar;
	class Gui;

	// Type aliases
	using String = std::string;

	// Shared pointer type alias
	template<typename T>
	using Ptr = std::shared_ptr<T>;

	// Signal stub for menu callbacks
	struct Signal {
		bool disconnect(int) { return true; }
	};

	// MenuBar stub - minimal interface to allow compilation
	class MenuBar {
	public:
		using Ptr = std::shared_ptr<MenuBar>;

		Signal onMenuItemClick;

		static Ptr create() { return std::make_shared<MenuBar>(); }

		void setSize(const std::string&, float) {}
		void addMenu(const std::string&) {}
		void addMenuItem(const std::vector<std::string>&) {}

		// Template to accept any function signature
		template<typename Func, typename... Args>
		unsigned int connectMenuItem(const std::vector<std::string>&, Func&&, Args&&...) { return 0; }

		bool getMenuItemEnabled(const std::vector<std::string>&) const { return true; }
		void setMenuItemEnabled(const std::vector<std::string>&, bool) {}
		void setMenuEnabled(const std::string&, bool) {}
		void removeAllMenus() {}
		void removeMenuItem(const std::vector<std::string>&) {}
		void removeMenuItems(const std::string&) {}
	};

	// Gui stub - minimal interface
	class Gui {
	public:
		// Constructor that accepts a window (for compatibility)
		Gui() = default;
		template<typename W>
		Gui(W&) {}  // Accept window reference but ignore it

		// Template to accept any shared_ptr type
		template<typename T>
		void add(std::shared_ptr<T>, const std::string&) {}

		template<typename T>
		std::shared_ptr<T> get(const std::string&) const {
			return std::make_shared<T>();
		}

		void draw() {}
		bool handleEvent(const sf::Event&) { return false; }
	};

	// Widget stub
	class Widget {
	public:
		using Ptr = std::shared_ptr<Widget>;
	};
}

#else
	// Desktop build: Use real TGUI
	#include <TGUI/TGUI.hpp>
#endif

#endif // BOE_COMPAT_TGUI_HPP
