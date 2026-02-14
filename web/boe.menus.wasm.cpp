/*
 * boe.menus.wasm.cpp
 * Minimal menu implementation for WASM builds
 * Creates menu event listener without visual menubar
 */

#include "boe.menu.hpp"
#include "boe.menus.hpp"
#include "boe.graphics.hpp"
#include "universe/universe.hpp"
#include "scenario/item.hpp"
#include <memory>
#include <unordered_map>
#include <iostream>

extern std::unordered_map<std::string, std::shared_ptr<iEventListener>> event_listeners;
extern cUniverse univ;

std::shared_ptr<OpenBoEMenu> menu_ptr;

void init_menubar() {
	std::cout << "[WASM] Initializing menu system (headless)" << std::endl;
	menu_ptr.reset(new OpenBoEMenu(univ));

	// Register as event listener (keyboard shortcuts will work)
	event_listeners["menubar"] = std::dynamic_pointer_cast<iEventListener>(menu_ptr);

	std::cout << "[WASM] Menu event listener registered" << std::endl;
}

void menu_activate() {
	// No-op in WASM (no visual menu)
}

void update_item_menu(eMenu which, const cItem& item) {
	// No-op in WASM (no visual menu)
}

void update_spell_menus() {
	if(menu_ptr)
		menu_ptr->update_spell_menus();
}

void update_monsters_menu() {
	if(menu_ptr)
		menu_ptr->update_monsters_menu();
}

void update_for_game_state() {
	// WASM doesn't need visual menu updates
	// But keep menu shortcuts active
}
