/*
 * web_stubs.cpp
 * Temporary stub implementations for web builds
 * These allow linking to succeed while full implementations are being developed
 */

#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <map>
#include <queue>
#include <optional>
#include <functional>
#include <emscripten.h>

// Define BOOST_FALLTHROUGH for C++17 fallthrough attribute
#ifndef BOOST_FALLTHROUGH
#define BOOST_FALLTHROUGH [[fallthrough]]
#endif
#include "fileio/xml-parser/ticpp.h"
#include "fileio/fileio.hpp"
#include "compat/graphics.hpp"
#include "location.hpp"
#include "gfx/tiling.hpp"
#include "fileio/resmgr/resmgr.hpp"
#include "fileio/resmgr/res_image.hpp"
#include "fileio/resmgr/res_font.hpp"
#include "fileio/resmgr/res_sound.hpp"
#include "fileio/resmgr/res_cursor.hpp"
#include "fileio/resmgr/res_dialog.hpp"
#include "fileio/resmgr/res_strings.hpp"
#include "universe/universe.hpp"
#include "dialogxml/widgets/control.hpp"
#include "tools/event_listener.hpp"
#include "tools/drawable.hpp"
#include "game/boe.fileio.hpp"
#include "game/boe.actions.hpp"
#include "game/boe.menus.hpp"
#include "tools/cursors.hpp"
#include "dialogxml/dialogs/strdlog.hpp"

// ============================================================
// Event queue for bridging JavaScript browser events to C++
// ============================================================
static std::queue<sf::Event> g_web_event_queue;

// Keyboard state tracking for sf::Keyboard::isKeyPressed()
static bool g_key_states[sf::Keyboard::KeyCount] = {false};

// Current mouse position (for sf::Mouse::getPosition) - in sf namespace
namespace sf {
	Vector2i g_current_mouse_pos(0, 0);

	// Implement isKeyPressed for WASM builds
	bool Keyboard::isKeyPressed(Key key) {
		if (key >= 0 && key < KeyCount) {
			return g_key_states[key];
		}
		return false;
	}
}

// Flag to request redraw when textures finish loading
static bool g_needs_redraw = false;

// Forward declare keymods_t and kb (defined later in this file)
#include "tools/keymods.hpp"
extern keymods_t kb;

// Mouse event type constants (must match events.js)
enum WebMouseEventType { MOUSE_PRESSED = 0, MOUSE_RELEASED = 1, MOUSE_MOVED = 2 };
// Key event type constants (must match events.js)
enum WebKeyEventType { KEY_PRESSED_WEB = 0, KEY_RELEASED_WEB = 1, KEY_TEXT_WEB = 2 };

// Forward declare cDialog to access topWindow for coordinate adjustment
class cDialog;

extern "C" {

EMSCRIPTEN_KEEPALIVE
void push_mouse_event(int type, int x, int y, int button) {
	// Update current mouse position
	sf::g_current_mouse_pos.x = x;
	sf::g_current_mouse_pos.y = y;

	sf::Event evt;
	switch(type) {
		case MOUSE_PRESSED:
			evt.type = sf::Event::MouseButtonPressed;
			evt.mouseButton.button = static_cast<sf::Mouse::Button>(button);
			evt.mouseButton.x = x;
			evt.mouseButton.y = y;
			break;
		case MOUSE_RELEASED:
			evt.type = sf::Event::MouseButtonReleased;
			evt.mouseButton.button = static_cast<sf::Mouse::Button>(button);
			evt.mouseButton.x = x;
			evt.mouseButton.y = y;
			break;
		case MOUSE_MOVED:
			evt.type = sf::Event::MouseMoved;
			evt.mouseMove.x = x;
			evt.mouseMove.y = y;
			break;
		default:
			return;
	}
	g_web_event_queue.push(evt);
}

EMSCRIPTEN_KEEPALIVE
void push_key_event(int type, int keyCode, int shift, int ctrl, int alt) {
	sf::Event evt;
	switch(type) {
		case KEY_PRESSED_WEB:
			evt.type = sf::Event::KeyPressed;
			evt.key.code = static_cast<sf::Keyboard::Key>(keyCode);
			evt.key.shift = (shift != 0);
			evt.key.control = (ctrl != 0);
			evt.key.alt = (alt != 0);
			evt.key.system = false;
			// Update keyboard state
			if (keyCode >= 0 && keyCode < sf::Keyboard::KeyCount) {
				g_key_states[keyCode] = true;
			}
			break;
		case KEY_RELEASED_WEB:
			evt.type = sf::Event::KeyReleased;
			evt.key.code = static_cast<sf::Keyboard::Key>(keyCode);
			evt.key.shift = (shift != 0);
			evt.key.control = (ctrl != 0);
			evt.key.alt = (alt != 0);
			evt.key.system = false;
			// Update keyboard state
			if (keyCode >= 0 && keyCode < sf::Keyboard::KeyCount) {
				g_key_states[keyCode] = false;
			}
			break;
		case KEY_TEXT_WEB:
			evt.type = sf::Event::TextEntered;
			evt.text.unicode = static_cast<uint32_t>(keyCode);
			break;
		default:
			return;
	}
	g_web_event_queue.push(evt);
}

EMSCRIPTEN_KEEPALIVE
void request_redraw() {
	g_needs_redraw = true;
}

} // extern "C"

namespace fs = std::filesystem;

// Forward declarations
class cDialog;

// Dialog functions now provided by src/dialogxml/dialogs/strdlog.cpp
// (showError, showWarning, showFatalError, give_help)

// NOTE: Enum stream operators now implemented in estreams.cpp
// NOTE: operator* for eSpell, eAlchemy, eEnchant now in spell.cpp, alchemy.cpp, enchant.cpp
// NOTE: get_str() now in res_strings.cpp

// showError(title, msg) now provided by strdlog.cpp

// NOTE: load_map() now implemented in map_parse.cpp

// Legacy format porting stubs (TODO: Implement when needed)
namespace legacy {
	struct scenario_data_type {};
	struct scen_item_data_type {};
	struct outdoor_record_type {};
	struct town_record_type {};
	struct big_tr_type {};
	struct ave_tr_type {};
	struct tiny_tr_type {};
	struct talking_record_type {};
	struct party_record_type {};
	struct pc_record_type {};
	struct current_town_type {};
}

// Legacy format porting functions now provided by src/porting.cpp

// Replay system stubs (TODO: Implement when replay system is added)
bool replaying = false;
bool recording = false;

ticpp::Element& pop_next_action(std::string expected_type) {
	// Stub - would return next action from replay log
	static ticpp::Element dummy("dummy");
	return dummy;
}

void decode_file(std::string data, fs::path file) {
	// Stub - would decode base64 file data
}

std::string encode_file(fs::path file) {
	// Stub - would encode file to base64
	return "";
}

// Global variables for async file dialog communication
static std::string g_dialog_result = "";
static bool g_dialog_pending = false;

// Called from JavaScript when dialog completes
extern "C" {
EMSCRIPTEN_KEEPALIVE
void set_dialog_result(const char* filename) {
	if(filename && filename[0] != '\0') {
		g_dialog_result = filename;
	} else {
		g_dialog_result = "";
	}
	g_dialog_pending = false;
}
}

// File navigation - HTML file dialog with IndexedDB backend
fs::path nav_get_party() {
	// Reset state
	g_dialog_result = "";
	g_dialog_pending = true;

	// Show load dialog (async JavaScript call)
	EM_ASM({
		Module._showLoadDialog().then(function(filename) {
			Module.ccall('set_dialog_result', null, ['string'], [filename || '']);
		}).catch(function(err) {
			console.error('Load dialog error:', err);
			Module.ccall('set_dialog_result', null, ['string'], ['']);
		});
	});

	// Wait for dialog to complete using ASYNCIFY-compatible sleep
	while(g_dialog_pending) {
		emscripten_sleep(16); // Sleep 16ms (60fps polling)
	}

	if(g_dialog_result.empty()) {
		return "";  // User cancelled
	}

	// Ensure the Saves directory exists in MEMFS
	fs::path saves_dir = "/temp/Saves";
	if (!fs::exists(saves_dir)) {
		fs::create_directories(saves_dir);
		std::cout << "[WASM] Created directory: " << saves_dir << std::endl;
	}

	fs::path result = "/temp/Saves/";
	result += g_dialog_result;
	return result;
}

fs::path nav_put_party(fs::path def) {
	std::cout << "[WASM] nav_put_party() called" << std::endl;

	// Reset state
	g_dialog_result = "";
	g_dialog_pending = true;

	// Show save dialog with default filename
	std::string defaultNameStr = def.filename().string();

	std::cout << "[WASM] Showing save dialog with default: " << defaultNameStr << std::endl;

	EM_ASM({
		console.log('[WASM JS] Calling Module._showSaveDialog');
		var defName = UTF8ToString($0);
		Module._showSaveDialog(defName).then(function(filename) {
			console.log('[WASM JS] Dialog returned:', filename);
			Module.ccall('set_dialog_result', null, ['string'], [filename || '']);
		}).catch(function(err) {
			console.error('Save dialog error:', err);
			Module.ccall('set_dialog_result', null, ['string'], ['']);
		});
	}, defaultNameStr.c_str());

	std::cout << "[WASM] Waiting for dialog to complete..." << std::endl;

	// Wait for dialog to complete using ASYNCIFY-compatible sleep
	while(g_dialog_pending) {
		emscripten_sleep(16); // Sleep 16ms (60fps polling)
	}

	std::cout << "[WASM] Dialog completed, result: " << g_dialog_result << std::endl;

	if(g_dialog_result.empty()) {
		return "";  // User cancelled
	}

	// Ensure the Saves directory exists in MEMFS
	fs::path saves_dir = "/temp/Saves";
	if (!fs::exists(saves_dir)) {
		fs::create_directories(saves_dir);
		std::cout << "[WASM] Created directory: " << saves_dir << std::endl;
	}

	fs::path result = "/temp/Saves/";
	result += g_dialog_result;
	return result;
}

// MEMFS to IndexedDB sync callback (called after save_party writes file)
extern "C" {
EMSCRIPTEN_KEEPALIVE
void sync_save_to_indexeddb(const char* filename) {
	EM_ASM_({
		var fname = UTF8ToString($0);
		Module._syncSaveToIndexedDB(fname);
	}, filename);
}

}

// Forward declarations for C++ globals
extern cUniverse univ;
extern cCustomGraphics spec_scen_g;
extern eGameMode overall_mode;
extern fs::path store_chose_auto;

extern "C" {

// Autosave function called from JavaScript
EMSCRIPTEN_KEEPALIVE
void save_party_to_autosave(const char* path) {
	fs::path target_path(path);

	// Ensure directory exists
	fs::path dir = target_path.parent_path();
	if (!dir.empty() && !fs::exists(dir)) {
		fs::create_directories(dir);
	}

	// Perform the save
	if(save_party_force(univ, target_path)) {
		// Sync to IndexedDB
		std::string filename = target_path.filename().string();
		sync_save_to_indexeddb(filename.c_str());
	} else {
		std::cerr << "Autosave failed for path: " << path << std::endl;
	}
}

// Load function called from JavaScript
// This provides a clean, synchronous entry point for loading from a known MEMFS path
// JavaScript handles all async operations (dialog, IndexedDB) before calling this
EMSCRIPTEN_KEEPALIVE
void wasm_load_from_path(const char* path_cstr) {
	std::cout << "[WASM] wasm_load_from_path: " << path_cstr << std::endl;

	fs::path file_to_load(path_cstr);

	// Ensure file exists
	if(!fs::exists(file_to_load)) {
		std::cerr << "[WASM] ERROR: File not found: " << path_cstr << std::endl;
		showError("Load failed: File not found");
		return;
	}

	std::cout << "[WASM] File exists, loading..." << std::endl;
	set_cursor(watch_curs);

	// Load the party (no file picking, just load from path)
	if(!load_party(file_to_load, univ, spec_scen_g)) {
		std::cerr << "[WASM] ERROR: load_party failed" << std::endl;
		restore_cursor();
		return;
	}

	std::cout << "[WASM] load_party succeeded, finishing up..." << std::endl;

	// Finish the load process
	finish_load_party();
	if(overall_mode != MODE_STARTUP)
		post_load();

	// Track the loaded file
	if(store_chose_auto.empty()){
		univ.file = file_to_load;
	} else {
		// Make sure when you choose an autosave, your next manual save overwrites the main file, not the autosave.
		univ.file = store_chose_auto;
	}

	menu_activate();
	restore_cursor();

	std::cout << "[WASM] Load complete!" << std::endl;
}
}

// flip_short and other endian conversion functions now provided by src/porting.cpp

void record_action(std::string action_type, std::string inner_text, bool cdata) {
	// Stub - would record action to replay log
}

std::string info_from_action(ticpp::Element& action) {
	// Stub - would extract info string from action element
	return "";
}

// UI and dialog stubs (TODO: Implement with HTML/Canvas)
// Note: cursor_type and Cursor are now defined in cursors.hpp (included via res_cursor.hpp)

void set_cursor(cursor_type type) {
	// Map BoE cursor types to original game cursor images
	const char* cursorFile = nullptr;
	int hotX = 8, hotY = 8;  // Default hotspot (center of typical 16x16 cursor)

	switch(type) {
		case wand_curs:
			cursorFile = "wand.gif"; hotX = 4; hotY = 12; break;
		case eyedropper_curs:
			cursorFile = "eyedropper.gif"; hotX = 4; hotY = 12; break;
		case brush_curs:
			cursorFile = "brush.gif"; hotX = 4; hotY = 12; break;
		case spray_curs:
			cursorFile = "spraycan.gif"; hotX = 4; hotY = 12; break;
		case eraser_curs:
			cursorFile = "eraser.gif"; hotX = 4; hotY = 12; break;
		case topleft_curs:
			cursorFile = "topleft.gif"; hotX = 0; hotY = 0; break;
		case bottomright_curs:
			cursorFile = "bottomright.gif"; hotX = 15; hotY = 15; break;
		case hand_curs:
			cursorFile = "hand.gif"; hotX = 8; hotY = 4; break;
		case NW_curs:
			cursorFile = "NW.gif"; hotX = 8; hotY = 8; break;
		case N_curs:
			cursorFile = "N.gif"; hotX = 8; hotY = 8; break;
		case NE_curs:
			cursorFile = "NE.gif"; hotX = 8; hotY = 8; break;
		case W_curs:
			cursorFile = "W.gif"; hotX = 8; hotY = 8; break;
		case wait_curs:
			cursorFile = "wait.gif"; hotX = 8; hotY = 8; break;
		case E_curs:
			cursorFile = "E.gif"; hotX = 8; hotY = 8; break;
		case SW_curs:
			cursorFile = "SW.gif"; hotX = 8; hotY = 8; break;
		case S_curs:
			cursorFile = "S.gif"; hotX = 8; hotY = 8; break;
		case SE_curs:
			cursorFile = "SE.gif"; hotX = 8; hotY = 8; break;
		case sword_curs:
			cursorFile = "sword.gif"; hotX = 8; hotY = 8; break;
		case boot_curs:
			cursorFile = "boot.gif"; hotX = 4; hotY = 12; break;
		case drop_curs:
			cursorFile = "drop.gif"; hotX = 8; hotY = 8; break;
		case target_curs:
			cursorFile = "target.gif"; hotX = 8; hotY = 8; break;
		case talk_curs:
			cursorFile = "talk.gif"; hotX = 8; hotY = 8; break;
		case key_curs:
			cursorFile = "key.gif"; hotX = 4; hotY = 4; break;
		case look_curs:
			cursorFile = "look.gif"; hotX = 8; hotY = 8; break;
		case bucket_curs:
			cursorFile = "bucket.gif"; hotX = 4; hotY = 12; break;
		case watch_curs:
			cursorFile = "watch.gif"; hotX = 8; hotY = 8; break;
		case text_curs:
			// Use CSS text cursor for text input
			EM_ASM(Module.canvas.style.cursor = 'text');
			return;
		default:
			// Use default cursor
			EM_ASM(Module.canvas.style.cursor = 'default');
			return;
	}

	if(cursorFile) {
		EM_ASM_({
			var filename = UTF8ToString($0);
			var hotX = $1;
			var hotY = $2;
			var cursorUrl = 'rsrc/cursors/' + filename;
			Module.canvas.style.cursor = 'url(' + cursorUrl + ') ' + hotX + ' ' + hotY + ', auto';
		}, cursorFile, hotX, hotY);
	}
}

// showFatalError now provided by strdlog.cpp

// Note: cControl, cTextMsg, cChoiceDlog now provided by dialogxml system

// Preferences system stubs
int get_int_pref(std::string key, int defaultVal) {
	return defaultVal;
}

double get_float_pref(std::string key, double defaultVal) {
	return defaultVal;
}

// Simple preferences implementation for WASM with localStorage persistence
static std::map<std::string, bool> boolPrefs;
static std::map<std::string, int> intPrefs;
static std::map<std::string, double> doublePrefs;
static std::map<std::string, std::vector<int>> iarrayPrefs;

// Load prefs from localStorage on first access
static bool prefsInitialized = false;
static void initPrefs() {
	if (prefsInitialized) return;
	prefsInitialized = true;

	// Load ReceivedHelp array from localStorage
	char* jsonStr = (char*)EM_ASM_PTR({
		try {
			var stored = localStorage.getItem('BoE_ReceivedHelp');
			if (!stored) return 0;
			var len = lengthBytesUTF8(stored) + 1;
			var ptr = _malloc(len);
			stringToUTF8(stored, ptr, len);
			return ptr;
		} catch(e) {
			console.error('Failed to load ReceivedHelp from localStorage:', e);
			return 0;
		}
	});

	if (jsonStr) {
		std::string json(jsonStr);
		free(jsonStr);

		// Simple JSON array parser
		if (json.length() > 2 && json[0] == '[' && json.back() == ']') {
			json = json.substr(1, json.length() - 2); // Remove [ ]
			std::istringstream iss(json);
			std::string token;
			while (std::getline(iss, token, ',')) {
				try {
					iarrayPrefs["ReceivedHelp"].push_back(std::stoi(token));
				} catch(...) {}
			}
		}

		if (!iarrayPrefs["ReceivedHelp"].empty()) {
			printf("[PREFS] Loaded %zu ReceivedHelp items from localStorage\n",
				iarrayPrefs["ReceivedHelp"].size());
		}
	}
}

// Save ReceivedHelp to localStorage
static void saveReceivedHelp() {
	auto& arr = iarrayPrefs["ReceivedHelp"];
	if (arr.empty()) return;

	// Build JSON array string
	std::string json = "[";
	for (size_t i = 0; i < arr.size(); i++) {
		if (i > 0) json += ",";
		json += std::to_string(arr[i]);
	}
	json += "]";

	EM_ASM_({
		var jsonStr = UTF8ToString($0);
		try {
			localStorage.setItem('BoE_ReceivedHelp', jsonStr);
		} catch(e) {
			console.error('Failed to save ReceivedHelp to localStorage:', e);
		}
	}, json.c_str());
}

bool get_bool_pref(std::string key, bool defaultVal) {
	initPrefs();
	auto it = boolPrefs.find(key);
	return it != boolPrefs.end() ? it->second : defaultVal;
}

void set_pref(std::string key, bool val) {
	boolPrefs[key] = val;
}

void set_pref(std::string key, int val) {
	intPrefs[key] = val;
}

void set_pref(std::string key, double val) {
	doublePrefs[key] = val;
}

std::vector<int> get_iarray_pref(std::string key) {
	initPrefs();
	auto it = iarrayPrefs.find(key);
	return it != iarrayPrefs.end() ? it->second : std::vector<int>();
}

void append_iarray_pref(std::string key, int value) {
	initPrefs();
	iarrayPrefs[key].push_back(value);
	printf("[PREFS] append_iarray_pref('%s', %d) - new size: %zu\n",
		key.c_str(), value, iarrayPrefs[key].size());

	// Auto-save ReceivedHelp to localStorage
	if (key == "ReceivedHelp") {
		saveReceivedHelp();
	}
}

void clear_pref(std::string key) {
	boolPrefs.erase(key);
	intPrefs.erase(key);
	doublePrefs.erase(key);
	iarrayPrefs.erase(key);

	if (key == "ReceivedHelp") {
		EM_ASM({ localStorage.removeItem('BoE_ReceivedHelp'); });
	}
}

bool sync_prefs() {
	// Save happens automatically in append_iarray_pref
	return true;
}

void set_clipboard(std::string text) {
	// Stub - would copy text to clipboard
	std::cout << "Copy to clipboard: " << text << std::endl;
}

// Window management stubs
void setWindowFloating(sf::Window& win, bool floating) {
	// Stub - would set window floating state
}

void makeFrontWindow(sf::Window& win, sf::Window& parent) {
	// Stub - would bring window to front
}

// UI scaling stub
double fallback_scale() {
	return 1.0;  // No scaling for web initially
}

// Menu update stubs
void adjust_spell_menus() {
	// Stub - would update spell menus
}

void adjust_monst_menu() {
	// Stub - would update monster menu
}

// ePicType now provided by dialogxml/widgets/pictypes.hpp

// custom_choice_dialog now provided by src/dialogxml/dialogs/3choice.cpp

// pick_race_abil and spend_xp now provided by src/pcedit/pc.editors.cpp

// NOTE: cPictChoice now provided by dialogxml/dialogs/pictchoice.cpp
// NOTE: cLed now provided by dialogxml/widgets/led.cpp

// System beep stub
void beep() {
	// Stub - would make system beep sound
}

// Cursor management
void restore_cursor() {
	EM_ASM(Module.canvas.style.cursor = 'default');
}

// NOTE: cPattern now provided by pattern.cpp
// NOTE: cToolbar and UI::toolbar now provided by boe.ui.cpp

// Replay system function
bool has_next_action(std::string expected_type) {
	// Stub - would check if there's a next action in replay log
	return false;
}

// Event polling function - dequeues from the web event queue
bool pollEvent(sf::Window& window, sf::Event& event) {
	static int poll_log_count = 0;
	static int last_queue_size = -1;
	int queue_size = g_web_event_queue.size();

	if(g_web_event_queue.empty()) return false;
	event = g_web_event_queue.front();
	g_web_event_queue.pop();

	// Track modifier keys
	if(event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
		if(kb.handleModifier(event)) return false; // consume pure modifier events
	}

	// Adjust mouse coordinates when a dialog is active (subtract draw offset)
	if(event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
		static int mouse_log_count = 0;
		if(mouse_log_count < 3) {
			std::cout << "pollEvent: Mouse " << (event.type == sf::Event::MouseButtonPressed ? "pressed" : "released")
			          << " at (" << event.mouseButton.x << "," << event.mouseButton.y << ")" << std::endl;
			mouse_log_count++;
		}
		auto& win = dynamic_cast<sf::RenderWindow&>(window);
		int offX = win.getDrawOffsetX();
		int offY = win.getDrawOffsetY();
		if(offX != 0 || offY != 0) {
			event.mouseButton.x -= offX;
			event.mouseButton.y -= offY;
		}
	} else if(event.type == sf::Event::MouseMoved) {
		auto& win = dynamic_cast<sf::RenderWindow&>(window);
		int offX = win.getDrawOffsetX();
		int offY = win.getDrawOffsetY();
		if(offX != 0 || offY != 0) {
			event.mouseMove.x -= offX;
			event.mouseMove.y -= offY;
		}
	}

	return true;
}

// Global background array (defined in tiling.hpp)
tessel_ref_t bg[256];

// Replay system global variables
int replay_fps_limit = 30;
fs::path log_file;

// Tiling pattern storage
struct TesselData {
	std::string filename;
	int srcX, srcY, srcW, srcH;
};
static std::map<int, TesselData> g_tessel_map;
static int g_next_tessel_key = 1;

tessel_ref_t prepareForTiling(sf::Texture& srcImg, rectangle srcRect) {
	tessel_ref_t ref;
	ref.key = g_next_tessel_key++;
	TesselData data;
	data.filename = srcImg.getFilename();
	data.srcX = srcRect.left;
	data.srcY = srcRect.top;
	data.srcW = srcRect.right - srcRect.left;
	data.srcH = srcRect.bottom - srcRect.top;
	g_tessel_map[ref.key] = data;
	return ref;
}

// Rendering functions - tile a pattern texture across a rectangle
void tileImage(sf::RenderTarget& target, rectangle rect, tessel_ref_t tessel_ref, sf::BlendMode mode) {
	auto it = g_tessel_map.find(tessel_ref.key);
	if(it == g_tessel_map.end()) {
		// No tessel data - draw a solid dark fill as fallback
		int dstX = rect.left;
		int dstY = rect.top;
		int dstW = rect.right - rect.left;
		int dstH = rect.bottom - rect.top;
		EM_ASM_({
			var ctx = Module.canvas.getContext('2d');
			ctx.fillStyle = 'rgb(66, 66, 66)';
			ctx.fillRect($0, $1, $2, $3);
		}, dstX, dstY, dstW, dstH);
		return;
	}

	const TesselData& td = it->second;

	// Get draw offset from the RenderWindow if possible
	int offX = 0, offY = 0;
	auto* rw = dynamic_cast<sf::RenderWindow*>(&target);
	if(rw) {
		offX = rw->getDrawOffsetX();
		offY = rw->getDrawOffsetY();
	}

	// Pre-compute values to pass to JS (EM_ASM_ has limited arg count)
	int dstX = rect.left + offX;
	int dstY = rect.top + offY;
	int dstW = rect.right - rect.left;
	int dstH = rect.bottom - rect.top;

	// Use EM_JS-style approach: pack all 9 args by storing source info first
	// EM_ASM_ has trouble with commas in JS object literals, so use array notation
	EM_ASM_({
		if(!Module._tileArgs) Module._tileArgs = [];
		Module._tileArgs[0] = $0;
		Module._tileArgs[1] = $1;
		Module._tileArgs[2] = $2;
		Module._tileArgs[3] = $3;
	}, td.srcX, td.srcY, td.srcW, td.srcH);

	// Then do the actual tiling with dest info + filename
	EM_ASM_({
		var ctx = Module.canvas.getContext('2d');
		var filename = UTF8ToString($0);
		var dstX = $1;
		var dstY = $2;
		var dstW = $3;
		var dstH = $4;
		var srcX = Module._tileArgs[0];
		var srcY = Module._tileArgs[1];
		var srcW = Module._tileArgs[2];
		var srcH = Module._tileArgs[3];

		if(!Module.textureCache) Module.textureCache = {};
		if(!Module.textureCache[filename]) {
			try {
				var data = FS.readFile(filename);
				var blob = new Blob([data]);
				var url = URL.createObjectURL(blob);
				var img = new Image();
				img.src = url;
				Module.textureCache[filename] = img;
			} catch(e) {
				ctx.fillStyle = 'rgb(66, 66, 66)';
				ctx.fillRect(dstX, dstY, dstW, dstH);
				return;
			}
		}

		var img = Module.textureCache[filename];
		if(img.complete && img.naturalWidth > 0 && srcW > 0 && srcH > 0) {
			if(!Module.tileCanvas) {
				Module.tileCanvas = document.createElement('canvas');
			}
			Module.tileCanvas.width = srcW;
			Module.tileCanvas.height = srcH;
			var tileCtx = Module.tileCanvas.getContext('2d');
			tileCtx.drawImage(img, srcX, srcY, srcW, srcH, 0, 0, srcW, srcH);

			var pattern = ctx.createPattern(Module.tileCanvas, 'repeat');
			ctx.save();
			ctx.translate(dstX, dstY);
			ctx.fillStyle = pattern;
			ctx.fillRect(0, 0, dstW, dstH);
			ctx.restore();
		} else {
			ctx.fillStyle = 'rgb(66, 66, 66)';
			ctx.fillRect(dstX, dstY, dstW, dstH);
		}
	}, td.filename.c_str(), dstX, dstY, dstW, dstH);
}

// Menu functions
void hideMenuBar() {
	// Stub - would hide menu bar in web
}

// init_menubar now provided by web/boe.menus.wasm.cpp

// Dialog functions
// once_dialog is now provided by src/dialogxml/dialogs/3choice.cpp

int get_num_response(short min, short max, std::string prompt, std::vector<std::string> opts, std::optional<short> def, short help, std::string, bool* cancelled) {
	// JavaScript fallback for numeric input dialog (Boost-free alternative)
	int result = EM_ASM_INT({
		var promptText = UTF8ToString($0);
		var input = window.prompt(promptText + " (" + $1 + " to " + $2 + "):", $3);
		if (input === null) {
			if ($4) HEAP8[$4] = 1; // Set cancelled flag
			return -1;
		}
		var num = parseInt(input);
		if (isNaN(num) || num < $1 || num > $2) {
			alert("Invalid number. Please enter a value between " + $1 + " and " + $2 + ".");
			if ($4) HEAP8[$4] = 1; // Set cancelled flag
			return -1;
		}
		if ($4) HEAP8[$4] = 0; // Clear cancelled flag
		return num;
	}, prompt.c_str(), min, max, (def ? *def : min), cancelled);
	return result;
}

// Forward declarations
class cDialog;
// Note: ePicType is already defined in pictypes.hpp as non-scoped enum

// cStrDlog class now provided by src/dialogxml/dialogs/strdlog.cpp

// Note: iComponent, iEventListener, iDrawable are now defined in their respective headers

// Note: cScrollbar is now implemented in dialogxml/widgets/scrollbar.cpp

// More menu and UI functions
void adjust_window_for_menubar(int mode, unsigned int width, unsigned int height) {}
void showMenuBar() {}
unsigned int getMenubarHeight() { return 0; }
// REMOVED get_bool_pref stub - use real implementation from prefs.cpp

// cDrawableManager now provided by src/tools/drawable_manager.cpp

// Global variables
tessel_ref_t bw_pats[128];
bool record_verbose = false;
bool replay_verbose = false;
std::string last_action_type;
bool replay_error = false;
bool replay_strict = false;
bool replay_warning = false;

// Additional functions and classes
struct word_rect_t {};
class cItem;
class cCreature;
class cScenario;

void record_click_talk_rect(word_rect_t rect, bool val) {}
void record_action(std::string type, std::map<std::string, std::string> attrs) {}
// display_pc and display_alchemy now provided by src/pcedit/pc.editors.cpp
// menu_activate now provided by web/boe.menus.wasm.cpp

// Keyboard modifier implementations (keymods_t declared in keymods.hpp, included above)

bool keymods_t::isShiftPressed() const { return shift; }
bool keymods_t::isSystemPressed() const { return ctrl; }
bool keymods_t::isCtrlPressed() const { return ctrl; }
bool keymods_t::isMetaPressed() const { return meta; }
bool keymods_t::isAltPressed() const { return alt; }
bool keymods_t::isUpPressed() const { return up; }
bool keymods_t::isDownPressed() const { return down; }
bool keymods_t::isLeftPressed() const { return left; }
bool keymods_t::isRightPressed() const { return right; }

bool keymods_t::handleModifier(const sf::Event& evt) {
	if(evt.type != sf::Event::KeyPressed && evt.type != sf::Event::KeyReleased) return false;
	using Key = sf::Keyboard::Key;
	bool newState = evt.type == sf::Event::KeyPressed;
	switch(evt.key.code) {
		case Key::LShift: case Key::RShift: shift = newState; break;
		case Key::LAlt: case Key::RAlt: alt = newState; break;
		case Key::LControl: case Key::RControl: ctrl = newState; break;
		case Key::LSystem: case Key::RSystem: meta = newState; break;
		case Key::Left: left = newState; return false;
		case Key::Right: right = newState; return false;
		case Key::Up: up = newState; return false;
		case Key::Down: down = newState; return false;
		default: return false;
	}
	return true;
}

keymods_t kb;

char keyToChar(sf::Keyboard::Key key, bool shift) {
	using kb = sf::Keyboard;
	switch(key) {
		case kb::A: return shift ? 'A' : 'a';
		case kb::B: return shift ? 'B' : 'b';
		case kb::C: return shift ? 'C' : 'c';
		case kb::D: return shift ? 'D' : 'd';
		case kb::E: return shift ? 'E' : 'e';
		case kb::F: return shift ? 'F' : 'f';
		case kb::G: return shift ? 'G' : 'g';
		case kb::H: return shift ? 'H' : 'h';
		case kb::I: return shift ? 'I' : 'i';
		case kb::J: return shift ? 'J' : 'j';
		case kb::K: return shift ? 'K' : 'k';
		case kb::L: return shift ? 'L' : 'l';
		case kb::M: return shift ? 'M' : 'm';
		case kb::N: return shift ? 'N' : 'n';
		case kb::O: return shift ? 'O' : 'o';
		case kb::P: return shift ? 'P' : 'p';
		case kb::Q: return shift ? 'Q' : 'q';
		case kb::R: return shift ? 'R' : 'r';
		case kb::S: return shift ? 'S' : 's';
		case kb::T: return shift ? 'T' : 't';
		case kb::U: return shift ? 'U' : 'u';
		case kb::V: return shift ? 'V' : 'v';
		case kb::W: return shift ? 'W' : 'w';
		case kb::X: return shift ? 'X' : 'x';
		case kb::Y: return shift ? 'Y' : 'y';
		case kb::Z: return shift ? 'Z' : 'z';
		case kb::Num1: return shift ? '!' : '1';
		case kb::Num2: return shift ? '@' : '2';
		case kb::Num3: return shift ? '#' : '3';
		case kb::Num4: return shift ? '$' : '4';
		case kb::Num5: return shift ? '%' : '5';
		case kb::Num6: return shift ? '^' : '6';
		case kb::Num7: return shift ? '&' : '7';
		case kb::Num8: return shift ? '*' : '8';
		case kb::Num9: return shift ? '(' : '9';
		case kb::Num0: return shift ? ')' : '0';
		case kb::Tilde: return shift ? '~' : '`';
		case kb::Hyphen: return shift ? '_' : '-';
		case kb::Equal: return shift ? '+' : '=';
		case kb::LBracket: return shift ? '{' : '[';
		case kb::RBracket: return shift ? '}' : ']';
		case kb::Semicolon: return shift ? ':' : ';';
		case kb::Quote: return shift ? '"' : '\'';
		case kb::Comma: return shift ? '<' : ',';
		case kb::Period: return shift ? '>' : '.';
		case kb::Slash: return shift ? '?' : '/';
		case kb::Backslash: return shift ? '|' : '\\';
		case kb::Tab: return '\t';
		case kb::Space: return ' ';
		case kb::Enter: return '\n';
		case kb::Backspace: return '\b';
		case kb::Delete: return '\x7f';
		default: break;
	}
	return '\0';
}

// String choice dialog
struct cStringChoice {
	cStringChoice(const std::vector<std::string>& choices, std::string title, cDialog* parent, bool cancel);
	size_t show(unsigned long initial_sel);
};

cStringChoice::cStringChoice(const std::vector<std::string>& choices, std::string title, cDialog* parent, bool cancel) {}
size_t cStringChoice::show(unsigned long initial_sel) { return 0; }

// File navigation
fs::path nav_get_rsrc(std::initializer_list<std::string> exts) { return ""; }
void nav_put_rsrc(std::initializer_list<std::string> exts, fs::path def) {}

// Replay and action functions
std::string next_action_type() { return ""; }
// edit_stuff_done now provided by src/pcedit/pc.editors.cpp

// REMOVED PREFERENCE STUBS - use real implementations from prefs.cpp
// (These stubs were preventing preferences from persisting!)

// Window management
void makeFrontWindow(sf::Window& win) {}

// Initialization functions
void set_up_apple_events() {}
void init_tiling() {
	// Set up background tile patterns for dialogs
	// This mirrors the desktop init_tiling() from tiling.cpp
	static const location pat_offs[17] = {
		{0,3}, {1,1}, {2,1}, {2,0},
		{3,0}, {3,1}, {1,3}, {0,0},
		{0,2}, {1,2}, {0,1}, {2,2},
		{2,3}, {3,2}, {1,0}, {4,0}, {3,3}
	};
	static const int pat_i[17] = {
		2, 3, 4, 5, 6, 8, 9, 10,
		11,12,13,14,15,16,17,19,20
	};
	rectangle bg_rects[21];
	for(short i = 0; i < 17; i++){
		bg_rects[pat_i[i]] = {0,0,64,64};
		bg_rects[pat_i[i]].offset(64 * pat_offs[i].x,64 * pat_offs[i].y);
	}
	rectangle tmp_rect = bg_rects[19];
	tmp_rect.offset(0, 64);
	bg_rects[0] = bg_rects[1] = bg_rects[18] = bg_rects[7] = tmp_rect;
	bg_rects[0].right -= 32;
	bg_rects[0].bottom -= 32;
	bg_rects[1].left += 32;
	bg_rects[1].bottom -= 32;
	bg_rects[18].right -= 32;
	bg_rects[18].top += 32;
	bg_rects[7].left += 32;
	bg_rects[7].top += 32;

	sf::Texture& bg_gworld = *ResMgr::graphics.get("pixpats");
	sf::Texture& bw_gworld = *ResMgr::graphics.get("bwpats");

	rectangle bw_rect = {0,0,8,8};
	for(int i = 0; i < 21; i++) {
		if(i < 6) {
			bw_pats[i] = prepareForTiling(bw_gworld, bw_rect);
			bw_rect.offset(8,0);
		}
		bg[i] = prepareForTiling(bg_gworld, bg_rects[i]);
	}
	std::cerr << "init_tiling: Initialized " << g_tessel_map.size() << " tile patterns" << std::endl;
}
void init_fileio() {}
void init_spell_menus() {}

// Web-specific directory initialization (avoids filesystem::canonical which fails in browser)
// Declare external directory paths from fileio.cpp
extern fs::path progDir, tempDir, scenDir, replayDir, saveDir;

// add_resmgr_paths helper function (matches the one in fileio.cpp)
static void add_resmgr_paths(const fs::path& basePath) {
	ResMgr::graphics.pushPath(basePath/"graphics");
	ResMgr::cursors.pushPath(basePath/"cursors");
	ResMgr::fonts.pushPath(basePath/"fonts");
	ResMgr::strings.pushPath(basePath/"strings");
	ResMgr::sounds.pushPath(basePath/"sounds");
	ResMgr::dialogs.pushPath(basePath/"dialogs");
}

void init_directories(const char* exec_path) {
	// In the browser, we use Emscripten's virtual filesystem
	// Resources are preloaded to /data via --preload-file
	std::cerr << "init_directories called (web build - using virtual filesystem)" << std::endl;

	// Set progDir to root so progDir/data resolves to /data
	progDir = "/";

	// Set up temp directories (will use browser storage later)
	tempDir = "/temp";
	scenDir = tempDir / "Scenarios";
	replayDir = tempDir / "Replays";
	saveDir = tempDir / "Saves";

	// Create VFS directories so fs::create_directories() calls don't fail
	EM_ASM({
		try { FS.mkdir('/temp'); } catch(e) {}
		try { FS.mkdir('/temp/Scenarios'); } catch(e) {}
		try { FS.mkdir('/temp/Saves'); } catch(e) {}
		try { FS.mkdir('/temp/Replays'); } catch(e) {}
	});

	// Register resource manager paths
	add_resmgr_paths(progDir / "data");

	std::cerr << "Program directory: " << progDir << std::endl;
	std::cerr << "Resources will be loaded from: " << (progDir / "data") << std::endl;
	std::cerr << "Resource paths registered for graphics, fonts, sounds, etc." << std::endl;
}

// Additional replay and logging functions
void record_action(ticpp::Element& elem) {}
location location_from_action(ticpp::Element& elem) { return location(); }
short short_from_action(ticpp::Element& elem) { return 0; }
void start_log_file(std::string filename) {}

// Window and cursor
bool check_window_moved(sf::RenderWindow& win, int& x, int& y, std::string pref) {
	// WASM: Position tracking for map is handled in drag event handler (boe.main.cpp)
	// Desktop version would read window position from OS and save to prefs
	// No action needed here for WASM
	return false;
}
void obscureCursor() {
	EM_ASM(Module.canvas.style.cursor = 'none');
}

// Note: cKey is now defined in keycodes.hpp

class cTextField {
public:
	virtual ~cTextField() {}
	cTextField(iComponent& parent);
	void handleInput(cKey key, bool shift);
	void replay_selection(ticpp::Element& elem);
};

cTextField::cTextField(iComponent& parent) {}
void cTextField::handleInput(cKey key, bool shift) {}
void cTextField::replay_selection(ticpp::Element& elem) {}

// Button widget
// Note: eControlType is now defined in control.hpp
enum class eBtnType {};

// Note: cButton is now implemented in dialogxml/widgets/button.cpp

// Additional replay functions
word_rect_t word_rect_from_action(ticpp::Element& elem) { return word_rect_t(); }

// Documentation
void launchDocs(std::string page) {}
void launchURL(std::string url) {}

// Stack widget (page container)
class cStack {
public:
	virtual ~cStack() {}
	cStack(iComponent& parent);
	void changeSelectedPage(int delta, bool wrap);
	bool setPage(unsigned long page, bool animate);
	void setPageCount(unsigned long count);
	unsigned long getPage() const;
	unsigned long getPageCount() const;
	void fillTabOrder(std::vector<int>& first, std::vector<int>& second);
};

cStack::cStack(iComponent& parent) {}
void cStack::changeSelectedPage(int delta, bool wrap) {}
bool cStack::setPage(unsigned long page, bool animate) { return true; }
void cStack::setPageCount(unsigned long count) {}
unsigned long cStack::getPage() const { return 0; }
unsigned long cStack::getPageCount() const { return 0; }
void cStack::fillTabOrder(std::vector<int>& first, std::vector<int>& second) {}

// Additional widget classes
class cConnector {
public:
	virtual ~cConnector() {}
	cConnector(iComponent& parent);
};

cConnector::cConnector(iComponent& parent) {}

class cScrollPane {
public:
	virtual ~cScrollPane() {}
	cScrollPane(iComponent& parent);
};

cScrollPane::cScrollPane(iComponent& parent) {}

class cTilemap {
public:
	virtual ~cTilemap() {}
	cTilemap(iComponent& parent);
	void fillTabOrder(std::vector<int>& first, std::vector<int>& second);
};

cTilemap::cTilemap(iComponent& parent) {}
void cTilemap::fillTabOrder(std::vector<int>& first, std::vector<int>& second) {}

// NOTE: cContainer now provided by container.cpp

// Additional preferences function
std::string get_string_pref(std::string key, std::string defaultVal) {
	return defaultVal;
}

// Keyboard modifier flush
void keymods_t::flushModifiers() {
	shift = false;
	ctrl = false;
	alt = false;
	meta = false;
}

// Modal session for dialogs
// ModalSession is declared in winutil.hpp but we can't include it here
// due to the non-inline systemKey global. Instead we redefine the class
// with the same layout to provide implementations.
class ModalSession {
	void* session;
	sf::Window* parent;
public:
	void pumpEvents();
	ModalSession(sf::Window& win, sf::Window& parent);
	~ModalSession();
};

ModalSession::ModalSession(sf::Window& win, sf::Window& par) : session(nullptr), parent(&par) {}
ModalSession::~ModalSession() {}
void ModalSession::pumpEvents() {}

// Event polling overload
bool pollEvent(sf::Window* window, sf::Event& event) {
	if(window == nullptr) return false;
	return pollEvent(*window, event);
}

// Key from action
cKey key_from_action(ticpp::Element& elem) {
	return cKey();
}

// Next action line
int next_action_line() {
	return 0;
}

// Note: Cursor class is now defined in cursors.hpp (included via res_cursor.hpp)
// Cursor constructor implementation for web build
Cursor::Cursor(fs::path path, float hotX, float hotY) {
	// Stub - would load cursor from file
}

// zlib functions (gzstream needs these)
extern "C" {
	void* gzopen(const char* path, const char* mode) {
		return nullptr;
	}

	int gzclose(void* file) {
		return 0;
	}

	int gzread(void* file, void* buf, unsigned int len) {
		return 0;
	}

	int gzwrite(void* file, const void* buf, unsigned int len) {
		return 0;
	}
}
