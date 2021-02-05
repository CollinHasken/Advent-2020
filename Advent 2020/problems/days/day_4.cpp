#include "day_4.h"

#include "../common_includes.h"

PROBLEM_CLASS_CPP(4);

using property_name = char[4];

enum class PROPERTY_TYPE {
	BYR = 0,
	IYR,
	EYR,
	HGT,
	HCL,
	ECL,
	PID,
	CID,

	NUM
};

static const property_name PROPERTY_TYPE_STRING[static_cast<int>(PROPERTY_TYPE::NUM)] = {
	"byr",
	"iyr",
	"eyr",
	"hgt",
	"hcl",
	"ecl",
	"pid",
	"cid",
};

// Get the property type given a property name
//
// name:	The name of the property
//
// Returns the property type
static PROPERTY_TYPE property_type_string_to_type(const property_name name)
{
	for (int i = 0; i < static_cast<int>(PROPERTY_TYPE::NUM); ++i) {
		if (strncmp(name, PROPERTY_TYPE_STRING[i], 3) != 0) {
			continue;
		}
		return static_cast<PROPERTY_TYPE>(i);
	}
	return PROPERTY_TYPE::NUM;
}

// Passport class
class passport {
public:
	// Property of the passport
	struct property {
		// Constants for valid ranges
		static constexpr int		BIRTH_YEAR_MIN			= 1920;
		static constexpr int		BIRTH_YEAR_MAX			= 2002;
		static constexpr int		ISSUE_YEAR_MIN			= 2010;
		static constexpr int		ISSUE_YEAR_MAX			= 2020;
		static constexpr int		EXPIRATION_YEAR_MIN	= 2020;
		static constexpr int		EXPIRATION_YEAR_MAX	= 2030;
		static constexpr int		HEIGHT_CM_MIN			= 150;
		static constexpr int		HEIGHT_CM_MAX			= 193;
		static constexpr int		HEIGHT_IN_MIN			= 59;
		static constexpr int		HEIGHT_IN_MAX			= 76;
		static constexpr char	HAIR_COLOR_CHAR		= '#';
		static constexpr int		HAIR_COLOR_NUM_DIGS	= 6;
		static constexpr char	HAIR_COLOR_MIN_DIG	= '0';
		static constexpr char	HAIR_COLOR_MAX_DIG	= '9';
		static constexpr char	HAIR_COLOR_MIN_CHAR	= 'a';
		static constexpr char	HAIR_COLOR_MAX_CHAR	= 'f';
		static constexpr int		EYE_COLOR_VALID_NUM	= 7;
		static const char*		EYE_COLOR_VALID[EYE_COLOR_VALID_NUM];
		static constexpr int		PASSPORT_ID_DIGS		= 9;
		static constexpr char	PASSPORT_ID_MIN_DIG	= '0';
		static constexpr char	PASSPORT_ID_MAX_DIG	= '9';

		property(const property_name prop_name) : m_type(property_type_string_to_type(prop_name)), m_is_set(false), m_is_valid(false) {};

		void set_valid(const std::string& prop_val);

		bool m_is_set;
		bool m_is_valid;
		PROPERTY_TYPE m_type;
	};

	passport(std::ifstream& input);

	void set_prop(std::string& property_val);

	bool is_set_passport() const;
	bool is_valid_passport() const;

	property m_props[8] = {
		property("byr"),
		property("iyr"),
		property("eyr"),
		property("hgt"),
		property("hcl"),
		property("ecl"),
		property("pid"),
		property("cid"),
	};
};
const char* passport::property::EYE_COLOR_VALID[passport::property::EYE_COLOR_VALID_NUM] = { "amb", "blu", "brn", "gry", "grn", "hzl", "oth" };

// Determine and set whether a property is vaid based on the passed in value
//
// prop_val:	The value for the property
void passport::property::set_valid(const std::string& prop_val)
{
	switch (m_type)
	{
	case PROPERTY_TYPE::BYR:
	{
		int year = std::stoi(prop_val);
		m_is_valid = year >= BIRTH_YEAR_MIN && year <= BIRTH_YEAR_MAX;
	}
		break;
	case PROPERTY_TYPE::IYR:
	{
		int year = std::stoi(prop_val);
		m_is_valid = year >= ISSUE_YEAR_MIN && year <= ISSUE_YEAR_MAX;
	}
		break;
	case PROPERTY_TYPE::EYR:
	{
		int year = std::stoi(prop_val);
		m_is_valid = year >= EXPIRATION_YEAR_MIN && year <= EXPIRATION_YEAR_MAX;
	}
		break;
	case PROPERTY_TYPE::HGT:
	{
		std::string::size_type unit_pos;
		int length = std::stoi(prop_val, &unit_pos);
		std::string unit(prop_val.substr(unit_pos));
		if (unit.compare("cm") == 0) {
			m_is_valid = length >= HEIGHT_CM_MIN && length <= HEIGHT_CM_MAX;
		} else if (unit.compare("in") == 0){
			m_is_valid = length >= HEIGHT_IN_MIN && length <= HEIGHT_IN_MAX;
		}
	}
		break;
	case PROPERTY_TYPE::HCL:
	{
		m_is_valid = false;
		if (prop_val.front() != HAIR_COLOR_CHAR) {
			return;
		}

		if (prop_val.size() != HAIR_COLOR_NUM_DIGS + 1) {
			return;
		}

		for (int i = 1; i < HAIR_COLOR_NUM_DIGS + 1; ++i) {
			if (!(prop_val[i] >= HAIR_COLOR_MIN_DIG && prop_val[i] <= HAIR_COLOR_MAX_DIG) 
				&& !(prop_val[i] >= HAIR_COLOR_MIN_CHAR && prop_val[i] <= HAIR_COLOR_MAX_CHAR)) {
				return;
			}
		}
		m_is_valid = true;
	}
		break;
	case PROPERTY_TYPE::ECL:
	{
		m_is_valid = false;
		for (const char* color : EYE_COLOR_VALID) {
			if (prop_val.compare(color) == 0) {
				m_is_valid = true;
				break;
			}
		}
	}
		break;
	case PROPERTY_TYPE::PID:
	{
		m_is_valid = false;
		if (prop_val.size() != PASSPORT_ID_DIGS) {
			return;
		}

		for (int i = 0; i < PASSPORT_ID_DIGS; ++i) {
			if (!(prop_val[i] >= PASSPORT_ID_MIN_DIG && prop_val[i] <= PASSPORT_ID_MAX_DIG)) {
				return;
			}
		}
		m_is_valid = true;
	}
		break;
	case PROPERTY_TYPE::CID:
	case PROPERTY_TYPE::NUM:
	default:
		break;
	}
}

passport::passport(std::ifstream& input)
{
	while (!input.eof()) {
		std::string input_line;
		std::getline(input, input_line);

		if (input_line.empty()) {
			return;
		}

		std::string property_val;
		std::istringstream input_line_stream(input_line);

		// Set each property
		while (std::getline(input_line_stream, property_val, ' ')) {
			set_prop(property_val);
		}
	}
}

// Set a property for the passport
//
// property_val:	The property key and value line
void passport::set_prop(std::string& property_pair)
{
	std::size_t colon_pos = property_pair.find_first_of(':');
	if (colon_pos == std::string::npos) {
		return;
	}

	// Extract the property and value
	std::string prop = property_pair.substr(0, colon_pos);
	std::string val = property_pair.substr(colon_pos + 1);

	property_name prop_name;
	prop.copy(prop_name, 3);
	prop_name[3] = '\0';

	PROPERTY_TYPE type = property_type_string_to_type(prop_name);
	if (type == PROPERTY_TYPE::NUM) {
		return;
	}

	// Set the property
	for (property& prop : m_props) {
		if (prop.m_type != type) {
			continue;
		}
		prop.m_is_set = true;

		prop.set_valid(val);
		break;
	}
}

// Check if all the properties (optionally CID) are set
//
// Returns true if all (optionally CID) are set
bool passport::is_set_passport() const
{
	for (const property& prop : m_props) {
		if (!prop.m_is_set) {
			if (prop.m_type != PROPERTY_TYPE::CID) {
				return false;
			}
		}
	}
	return true;
}

// Check if all the properties (optionally CID) have valid values
//
// Returns true if all (optionally CID) have valid values
bool passport::is_valid_passport() const
{
	for (const property& prop : m_props) {
		if (!prop.m_is_valid) {
			if (prop.m_type != PROPERTY_TYPE::CID) {
				return false;
			}
		}
	}
	return true;
}


/*
* Detect which passports have all required fields 
* Passport data is validated in batch files (your puzzle input). Each passport 
* is represented as a sequence of key:value pairs separated by spaces or newlines.
* Passports are separated by blank lines. Missing cid is fine.
*/

// Count the number of valid passports - those that have all required fields. Treat cid as optional
void problem_1::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	int valid_passports = 0;
	while (!input.eof()) {
		passport new_id(input);
		// Only care if they have fields set
		if (new_id.is_set_passport()) {
			++valid_passports;
		}
	}
	input.close();

	output_answer(std::to_string(valid_passports));
}

/*
* You can continue to ignore the cid field, but each other field has strict rules about what values are valid for automatic validation:
* byr (Birth Year) - four digits; at least 1920 and at most 2002.
* iyr (Issue Year) - four digits; at least 2010 and at most 2020.
* eyr (Expiration Year) - four digits; at least 2020 and at most 2030.
* hgt (Height) - a number followed by either cm or in:
*   If cm, the number must be at least 150 and at most 193.
*   If in, the number must be at least 59 and at most 76.
* hcl (Hair Color) - a # followed by exactly six characters 0-9 or a-f.
* ecl (Eye Color) - exactly one of: amb blu brn gry grn hzl oth.
* pid (Passport ID) - a nine-digit number, including leading zeroes.
* cid (Country ID) - ignored, missing or not.
*/

// Count the number of valid passports - those that have all required fields and valid values
void problem_2::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	int valid_passports = 0;
	while (!input.eof()) {
		passport new_id(input);
		// Need to check if it's actually valid
		if (new_id.is_valid_passport()) {
			++valid_passports;
		}
	}
	input.close();

	output_answer(std::to_string(valid_passports));
}