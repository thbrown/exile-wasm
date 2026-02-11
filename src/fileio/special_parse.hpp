//
//  special_parse.hpp
//  BoE
//
//  Created by Celtic Minstrel on 14-12-02.
//
//

#ifndef BoE_special_parse_hpp
#define BoE_special_parse_hpp

#include "scenario/special.hpp"

#ifndef __EMSCRIPTEN__
#include <boost/spirit/include/classic.hpp>

enum eParseError {
	generic_error,
	double_def,
	expect_op,
	expect_sym,
	expect_dat,
	expect_eq,
	expect_int,
	expect_val,
	expect_nl,
	NUM_PARSE_ERR
};

namespace spirit = boost::spirit::classic;
typedef spirit::rule<> Rule;
typedef spirit::assertion<eParseError> Err;
typedef spirit::guard<eParseError> Guard;

class SpecialParser {
	using Iter = Rule::scanner_t::iterator_t;
	using ErrStatus = spirit::error_status<>;
	static void init_file(Iter, Iter);
	static void init_block(Iter, Iter);
	static void prep_add_symbol(Iter, Iter);
	static void add_symbol(int i);
	static void add_command(Iter, Iter);
	static void set_type(eSpecType type);
	static void skip_to(int i);
	static void for_sdf(Iter, Iter);
	static void for_pic(Iter, Iter);
	static void for_msg(Iter, Iter);
	static void for_ex1(Iter, Iter);
	static void for_ex2(Iter, Iter);
	static void for_goto(Iter, Iter);
	static void set_first(int i);
	static void set_second(int i);
	static void set_third(int i);
	static void next_line(Iter, Iter);
	static void maybe_throw(Iter, Iter);
	static std::string temp_symbol;
	static int cur_node, cur_fld;
	static cSpecial curSpec;
	static int lineno, last_line_start;
	static Iter file_start;
	static std::map<size_t, cSpecial> specials;
	static spirit::symbols<> defn;
	static Rule ws, eq, comment, symbol, symbol_ch, eol, op_assign;
	static Rule datcode, command, init_line, null_line, def_line, cmd_line, op_line, cmd_block, nodes_file;
	static Rule cmd_1st, cmd_2nd, cmd_3rd;
	static Err err, assert_op, assert_sym, assert_dat, assert_eq, assert_int, assert_val;
	static Guard guard;
	static bool grammar_built;
	static ErrStatus check_error(const Rule::scanner_t&, spirit::parser_error<eParseError, Iter>);
	static eParseError last_err;
public:
	SpecialParser();
	std::map<size_t,cSpecial> parse(std::string code, std::string context);
};

class xSpecParseError : public std::exception {
	static const char*const messages[NUM_PARSE_ERR];
	eParseError err;
	int line, col;
	std::string found, file;
	mutable const char* msg;
public:
	xSpecParseError(std::string found, eParseError expect, int line, int col, std::string file);
	~xSpecParseError() throw();
	const char* what() const throw();
};
#else
// WASM: Simple custom parser for .spec format
#include <sstream>
#include <cctype>

enum eParseError {
	generic_error,
	NUM_PARSE_ERR
};

class SpecialParser {
public:
	SpecialParser() {}

	std::map<size_t,cSpecial> parse(std::string code, std::string context) {
		std::map<size_t, cSpecial> specials;
		std::istringstream stream(code);
		std::string line;
		cSpecial* current = nullptr;
		size_t current_num = 0;

		while(std::getline(stream, line)) {
			// Skip empty lines and comments
			if(line.empty() || line[0] == '#') continue;

			// Trim whitespace
			size_t start = line.find_first_not_of(" \t\r");
			if(start == std::string::npos) continue;
			line = line.substr(start);

			if(line[0] == '@') {
				// New special node definition: @type-name = number
				size_t eq_pos = line.find('=');
				if(eq_pos != std::string::npos) {
					std::string type_str = line.substr(1, eq_pos - 1);
					// Trim type_str
					size_t type_end = type_str.find_last_not_of(" \t");
					if(type_end != std::string::npos) type_str = type_str.substr(0, type_end + 1);

					std::string num_str = line.substr(eq_pos + 1);
					current_num = std::stoi(num_str);

					specials[current_num] = cSpecial();
					current = &specials[current_num];
					current->type = parseSpecType(type_str);
				}
			} else if(current) {
				// Field definition: field_name val1, val2, ...
				size_t space_pos = line.find_first_of(" \t");
				if(space_pos != std::string::npos) {
					std::string field = line.substr(0, space_pos);
					std::string values_str = line.substr(space_pos + 1);

					// Parse comma-separated values
					std::vector<int> values;
					std::istringstream val_stream(values_str);
					std::string val;
					while(std::getline(val_stream, val, ',')) {
						// Trim and parse
						size_t val_start = val.find_first_not_of(" \t");
						if(val_start != std::string::npos) {
							values.push_back(std::stoi(val.substr(val_start)));
						}
					}

					// Assign to appropriate field
					assignField(*current, field, values);
				}
			}
		}

		return specials;
	}

private:
	eSpecType parseSpecType(const std::string& type) {
		// Opcode mappings from rsrc/strings/specials-opcodes.txt
		if(type == "set-sdf") return eSpecType::SET_SDF;
		if(type == "inc-sdf") return eSpecType::INC_SDF;
		if(type == "disp-msg") return eSpecType::DISPLAY_MSG;
		if(type == "start-shop") return eSpecType::ENTER_SHOP;
		if(type == "disp-sm-msg") return eSpecType::DISPLAY_SM_MSG;
		if(type == "flip-sdf") return eSpecType::FLIP_SDF;
		if(type == "story-dlog") return eSpecType::STORY_DIALOG;
		if(type == "block-move") return eSpecType::CANT_ENTER;
		if(type == "change-time") return eSpecType::CHANGE_TIME;
		if(type == "rest") return eSpecType::REST;
		if(type == "title-msg") return eSpecType::TITLED_MSG;
		if(type == "end-scen") return eSpecType::END_SCENARIO;
		if(type == "once-give-item") return eSpecType::ONCE_GIVE_ITEM;
		if(type == "once-give-spec-item") return eSpecType::ONCE_GIVE_SPEC_ITEM;
		if(type == "once") return eSpecType::ONCE_NULL;
		if(type == "once-set-sdf") return eSpecType::ONCE_SET_SDF;
		if(type == "once-disp-msg") return eSpecType::ONCE_DISPLAY_MSG;
		if(type == "once-dlog") return eSpecType::ONCE_DIALOG;
		if(type == "once-give-dlog") return eSpecType::ONCE_GIVE_ITEM_DIALOG;
		if(type == "once-out-encounter") return eSpecType::ONCE_OUT_ENCOUNTER;
		if(type == "once-town-encounter") return eSpecType::ONCE_TOWN_ENCOUNTER;
		if(type == "once-trap") return eSpecType::ONCE_TRAP;
		return eSpecType::NONE;
	}

	void assignField(cSpecial& spec, const std::string& field, const std::vector<int>& values) {
		if(field == "sdf" && values.size() >= 2) {
			spec.sd1 = values[0];
			spec.sd2 = values[1];
		} else if(field == "msg" && values.size() >= 3) {
			spec.m1 = values[0];
			spec.m2 = values[1];
			spec.m3 = values[2];
		} else if(field == "pic" && values.size() >= 2) {
			spec.pic = values[0];
			spec.pictype = values[1];
		} else if(field == "ex1" && values.size() >= 3) {
			spec.ex1a = values[0];
			spec.ex1b = values[1];
			spec.ex1c = values[2];
		} else if(field == "ex2" && values.size() >= 3) {
			spec.ex2a = values[0];
			spec.ex2b = values[1];
			spec.ex2c = values[2];
		} else if(field == "goto" && values.size() >= 1) {
			spec.jumpto = values[0];
		}
	}
};

class xSpecParseError : public std::exception {
public:
	xSpecParseError(std::string found, eParseError expect, int line, int col, std::string file) {}
	const char* what() const throw() { return "Special parse error"; }
};
#endif

#endif
