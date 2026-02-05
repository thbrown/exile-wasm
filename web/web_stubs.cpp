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
#include <optional>
#include <functional>
#include "fileio/xml-parser/ticpp.h"
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
#include "dialogxml/widgets/control.hpp"
#include "tools/event_listener.hpp"
#include "tools/drawable.hpp"

namespace fs = std::filesystem;

// Forward declarations
class cDialog;

// Dialog functions (TODO: Implement properly)
void showError(std::string msg, cDialog* parent) {
	std::cerr << "Error: " << msg << std::endl;
}

void showWarning(std::string msg, cDialog* parent) {
	std::cerr << "Warning: " << msg << std::endl;
}

void showWarning(std::string title, std::string msg, cDialog* parent) {
	std::cerr << "Warning [" << title << "]: " << msg << std::endl;
}

// Help system (TODO: Implement properly)
void give_help(short help1, short help2, bool force) {
	// Stub - would show help dialog
}

void give_help(short help1, short help2, cDialog& parent, bool force) {
	// Stub - would show help dialog
}

// NOTE: Enum stream operators now implemented in estreams.cpp
// NOTE: operator* for eSpell, eAlchemy, eEnchant now in spell.cpp, alchemy.cpp, enchant.cpp
// NOTE: get_str() now in res_strings.cpp

// Additional dialog function overload
void showError(std::string title, std::string msg, cDialog* parent) {
	std::cerr << "Error [" << title << "]: " << msg << std::endl;
}

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

void port_scenario(legacy::scenario_data_type* data) {
	// Stub - would convert legacy scenario format
}

void port_item_list(legacy::scen_item_data_type* data) {
	// Stub - would convert legacy item format
}

void port_out(legacy::outdoor_record_type* data) {
	// Stub - would convert legacy outdoor format
}

void port_town(legacy::town_record_type* data) {
	// Stub - would convert legacy town format
}

void port_t_d(legacy::big_tr_type* data) {
	// Stub - would convert legacy big terrain format
}

void port_ave_t(legacy::ave_tr_type* data) {
	// Stub - would convert legacy average terrain format
}

void port_tiny_t(legacy::tiny_tr_type* data) {
	// Stub - would convert legacy tiny terrain format
}

void port_talk_nodes(legacy::talking_record_type* data) {
	// Stub - would convert legacy talking nodes format
}

void port_party(legacy::party_record_type* data) {
	// Stub - would convert legacy party format
}

void port_pc(legacy::pc_record_type* data) {
	// Stub - would convert legacy PC format
}

void port_c_town(legacy::current_town_type* data) {
	// Stub - would convert legacy current town format
}

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

// File navigation stubs (TODO: Implement with HTML5 File API)
fs::path nav_get_party() {
	// Stub - would show file picker and return selected party file path
	return "";
}

fs::path nav_put_party(fs::path def) {
	// Stub - would show save dialog for party file
	return "";
}

// Endianness conversion stub (not needed on web - same endianness)
void flip_short(short* value) {
	// Stub - web builds don't need endianness conversion
	// Both WASM and typical data files are little-endian
}

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
	// Stub - would set cursor appearance
}

void showFatalError(std::string msg, cDialog* parent) {
	std::cerr << "Fatal Error: " << msg << std::endl;
}

// Note: cControl, cTextMsg, cChoiceDlog now provided by dialogxml system

// Preferences system stubs
int get_int_pref(std::string key, int defaultVal) {
	return defaultVal;
}

double get_float_pref(std::string key, double defaultVal) {
	return defaultVal;
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

// Forward declarations for dialog enums/types
enum class ePicType {
	// Stub - picture type enum
};

// Game dialog stubs
int custom_choice_dialog(std::array<std::string, 6>& choices, short numChoices, ePicType picType, std::array<short, 3>& pics, bool allowCancel, short defaultChoice, int helpId, cDialog* parent) {
	return 0;  // Stub - would show custom choice dialog
}

void pick_race_abil(class cPlayer* pc, short mode, cDialog* parent) {
	// Stub - would show race/ability picker
}

short spend_xp(short pc, short how_much, cDialog* parent) {
	return 0;  // Stub - would spend XP
}

// NOTE: cPictChoice now provided by dialogxml/dialogs/pictchoice.cpp
// NOTE: cLed now provided by dialogxml/widgets/led.cpp

// System beep stub
void beep() {
	// Stub - would make system beep sound
}

// Cursor management stub
void restore_cursor() {
	// Stub - would restore default cursor
}

// NOTE: cPattern now provided by pattern.cpp
// NOTE: cToolbar and UI::toolbar now provided by boe.ui.cpp

// Replay system function
bool has_next_action(std::string expected_type) {
	// Stub - would check if there's a next action in replay log
	return false;
}

// Event polling function
bool pollEvent(sf::Window& window, sf::Event& event) {
	// Stub - would poll for window events
	return false;
}

// Global background array (defined in tiling.hpp)
tessel_ref_t bg[256];

// Replay system global variables
int replay_fps_limit = 30;
fs::path log_file;

// Rendering functions
void tileImage(sf::RenderTarget& target, rectangle rect, tessel_ref_t tessel_ref, sf::BlendMode mode) {
	// Stub - would tile a pattern texture
}

// Menu functions
void hideMenuBar() {
	// Stub - would hide menu bar in web
}

void init_menubar() {
	// Stub - would initialize menu bar in web
}

// Dialog functions
class cDialog;
class cUniverse;
class cSpecial;
enum class eSpecCtxType;

bool once_dialog(cUniverse& univ, cSpecial& spec, eSpecCtxType ctx, cDialog* parent) {
	// Stub - would show a once-only dialog
	return false;
}

int get_num_response(short min, short max, std::string prompt, std::vector<std::string> opts, std::optional<short> def, short help, std::string, bool*) {
	// Stub - would show a numeric input dialog
	return 0;
}

// Forward declarations
class cDialog;
enum class ePicType;

// cStrDlog class (string dialog)
struct cStrDlog {
	cStrDlog(std::string title, std::string prompt, std::string defaultVal, short maxLen, ePicType picType, cDialog* parent);
	void show();
	bool setSound(int sound);
	bool setRecordHandler(std::function<void (cDialog&)> handler);
};

cStrDlog::cStrDlog(std::string title, std::string prompt, std::string defaultVal, short maxLen, ePicType picType, cDialog* parent) {}
void cStrDlog::show() {}
bool cStrDlog::setSound(int sound) { return true; }
bool cStrDlog::setRecordHandler(std::function<void (cDialog&)> handler) { return true; }

// Note: iComponent, iEventListener, iDrawable are now defined in their respective headers

// Note: cScrollbar is now implemented in dialogxml/widgets/scrollbar.cpp

// More menu and UI functions
void adjust_window_for_menubar(int mode, unsigned int width, unsigned int height) {}
void showMenuBar() {}
unsigned int getMenubarHeight() { return 0; }
bool get_bool_pref(std::string key, bool defaultVal) { return defaultVal; }

// Drawable manager
struct cDrawableManager {
	void draw_all();
};

void cDrawableManager::draw_all() {}

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
struct cItem {};
struct cCreature {};
struct cScenario {};

void record_click_talk_rect(word_rect_t rect, bool val) {}
void record_action(std::string type, std::map<std::string, std::string> attrs) {}
void put_item_info(cDialog& dlg, const cItem& item, const cScenario& scen) {}
void put_monst_info(cDialog& dlg, const cCreature& monst, const cScenario& scen) {}
void display_pc(short mode, short pc_num, cDialog* parent) {}
void display_alchemy(bool mode, cDialog* parent) {}
void menu_activate() {}

// Keyboard modifiers
struct keymods_t {
	bool isShiftPressed() const;
	bool isSystemPressed() const;
	bool isCtrlPressed() const;
	bool isMetaPressed() const;
	bool isAltPressed() const;
	void flushModifiers();
};

bool keymods_t::isShiftPressed() const { return false; }
bool keymods_t::isSystemPressed() const { return false; }
bool keymods_t::isCtrlPressed() const { return false; }
bool keymods_t::isMetaPressed() const { return false; }
bool keymods_t::isAltPressed() const { return false; }

keymods_t kb;

char keyToChar(sf::Keyboard::Key key, bool shift) { return '\0'; }

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
void edit_stuff_done() {}

// Preferences
void set_pref(std::string key, bool val) {}
void set_pref(std::string key, int val) {}
void set_pref(std::string key, double val) {}
void clear_pref(std::string key) {}
bool sync_prefs() { return true; }

// Window management
void makeFrontWindow(sf::Window& win) {}

// Initialization functions
void set_up_apple_events() {}
void init_tiling() {}
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
bool check_window_moved(sf::RenderWindow& win, int& x, int& y, std::string pref) { return false; }
void obscureCursor() {}

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
void keymods_t::flushModifiers() {}

// Modal session for dialogs
class ModalSession {
public:
	ModalSession(sf::Window& win, sf::Window& parent);
	~ModalSession();
};

ModalSession::ModalSession(sf::Window& win, sf::Window& parent) {}
ModalSession::~ModalSession() {}

// Event polling overload
bool pollEvent(sf::Window* window, sf::Event& event) {
	return false;
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
