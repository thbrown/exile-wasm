/*
 * boe.menus.wasm.cpp
 * Menu implementation for WASM builds.
 * Creates a keyboard-shortcut event listener and notifies the HTML menu bar
 * (web/menu.js / shell.html) of game-state changes via EM_ASM.
 */

#include "boe.menu.hpp"
#include "boe.menus.hpp"
#include "boe.consts.hpp"
#include "boe.graphics.hpp"
#include "universe/universe.hpp"
#include "scenario/item.hpp"
#include <emscripten.h>
#include <memory>
#include <unordered_map>
#include <iostream>

extern std::unordered_map<std::string, std::shared_ptr<iEventListener>> event_listeners;
extern cUniverse univ;
extern eGameMode overall_mode;
extern bool party_in_memory;

std::shared_ptr<OpenBoEMenu> menu_ptr;

void init_menubar() {
	std::cout << "[WASM] Initializing menu system (headless)" << std::endl;
	menu_ptr.reset(new OpenBoEMenu(univ));

	// Register as event listener (keyboard shortcuts will work)
	event_listeners["menubar"] = std::dynamic_pointer_cast<iEventListener>(menu_ptr);

	std::cout << "[WASM] Menu event listener registered" << std::endl;
}

void menu_activate() {
	// Notify the HTML menu bar to enable/disable menus based on game state
	EM_ASM_({
		if (typeof window.gameMenu !== 'undefined') {
			window.gameMenu.updateState($0 !== 0, $1 !== 0);
		}
	}, (int)(overall_mode == MODE_STARTUP), (int)party_in_memory);
}

void update_item_menu(eMenu which, const cItem& item) {
	// No-op in WASM (no visual menu)
}

void update_spell_menus() {
	if(menu_ptr)
		menu_ptr->update_spell_menus();
	// Notify the HTML menu bar to rebuild spell dropdown items
	EM_ASM({
		if (typeof window.gameMenu !== 'undefined') {
			window.gameMenu.refreshSpellMenus();
		}
	});
}

void update_monsters_menu() {
	if(menu_ptr)
		menu_ptr->update_monsters_menu();
}

void update_for_game_state() {
	// WASM doesn't need visual menu updates
	// But keep menu shortcuts active
}
