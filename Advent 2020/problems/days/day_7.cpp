#include "day_7.h"

#include "../common_includes.h"

#include <vector>
#include <map>
#include <algorithm>

PROBLEM_CLASS_CPP(7);

typedef int bag_type;

static const char* const BAG = "bag";
static const char* const SHINY_GOLD = "shinygold";
static const bag_type SHINY_GOLD_TYPE = 0;

static bag_type Current_bag_type = 1;
static std::map<std::string, bag_type> Bag_types;

// Get the next bag name from the input line
//
// bag_name:		(Output) The name of the bag
// input_stream:	The input line of the bag and possibly other bags
//
// Returns true if a name was found
static bool get_bag_name(std::string* bag_name, std::istream* input_stream)
{
	std::getline(*input_stream, *bag_name, ' ');
	std::string next_word;

	while (!input_stream->eof())
	{
		std::getline(*input_stream, next_word, ' ');

		// If this word is "bag", "bag,", "bag.", or "bags" then we've found the name
		if (next_word.find(BAG) == 0 && next_word.size() <= 5) {
			if (next_word.size() == 3 || (next_word.size() == 4 && next_word.back() == 's') || (next_word.back() == ',' || next_word.back() == '.')) {
				return true;
			}
		}
		// Otherwise this word is a part of the name
		bag_name->append(next_word);
	}
	return false;
}

// Get bag type for name
//
// bag_name:	The uniqueu name of a bag type
//
// Returns the bag type
static bag_type get_bag_type(const std::string& bag_name)
{
	auto bag_type_iter = Bag_types.find(bag_name);
	if (bag_type_iter == Bag_types.end()) {
		Bag_types.emplace(bag_name, Current_bag_type);
		return Current_bag_type++;
	}
	return bag_type_iter->second;
}

// Bag class that lists what types and how many are inside it
// and whether it can have gold
class bag {
public:
	enum class can_have_gold {
		YES,
		NO,
		UNKNOWN,
	};

	bag(const std::string& input_line);

	bag_type m_type;
	can_have_gold m_has_gold;
	std::vector<std::pair<bag_type, int>> m_bag_types_inside;
};

// Create a bag from input
bag::bag(const std::string& input_line)
{
	m_has_gold = can_have_gold::UNKNOWN;

	std::istringstream input_stream(input_line);

	// Get this bag
	std::string bag_name;
	get_bag_name(&bag_name, &input_stream);

	m_type = get_bag_type(bag_name);

	// Get to the next bag
	input_stream.ignore(32, ' ');

	// Get all the bags that can go within this one
	while (!input_stream.eof()) {
		int num_bags = 0;
		input_stream >> num_bags;
		if (input_stream.fail()) {
			return;
		}
		input_stream.ignore();

		// Get bag within's name and type
		get_bag_name(&bag_name, &input_stream);
		bag_type type = get_bag_type(bag_name);

		// Add the type and how many this can hold
		m_bag_types_inside.emplace_back(type, num_bags);

		if (input_stream.peek() == ' ') {
			input_stream.ignore();
		}
	}
}

// Comparotor to make the gold bag be sorted first
bool bag_lt(const bag& lhs, const bag& rhs) 
{
	return lhs.m_type < rhs.m_type;
}

// Determine if the bag can contain a shiny gold bag
// 
// cur_bag:	The bag to check if it can have gold
// bags:		The available bags and their types
//
// Returns true if the bag can hold a shiny gold bag
bool can_bag_have_gold(bag& cur_bag, std::vector<bag>& bags)
{
	// Already cached
	if (cur_bag.m_has_gold != bag::can_have_gold::UNKNOWN) {
		return cur_bag.m_has_gold == bag::can_have_gold::YES;
	}

	// Check each bag that this bag can contain
	for (std::pair<bag_type, int> bag_inside_type_pair : cur_bag.m_bag_types_inside) {
		bag& bag_inside = bags[bag_inside_type_pair.first];
		// If the bag inside is gold or can have gold, cache and return true
		if (bag_inside.m_type == SHINY_GOLD_TYPE || can_bag_have_gold(bag_inside, bags)) {
			cur_bag.m_has_gold = bag::can_have_gold::YES;
			return true;
		}
	}

	cur_bag.m_has_gold = bag::can_have_gold::NO;
	return false;
}

// Get total bags inside given bag
//
// cur_bag:	The bag to count contained bags
// bags:		The available bags and their types
int get_total_bags_inside(const bag& cur_bag, std::vector<bag>& bags)
{
	int bags_inside = 0;
	for (std::pair<bag_type, int> bag_inside_type_pair : cur_bag.m_bag_types_inside) {
		bag& bag_inside = bags[bag_inside_type_pair.first];
		bags_inside += bag_inside_type_pair.second * (get_total_bags_inside(bag_inside, bags) + 1);
	}

	return bags_inside;
}

/*
* You have a shiny gold bag. If you wanted to carry it in at least one other bag,
* how many different bag colors would be valid for the outermost bag?
* (In other words: how many colors can, eventually, contain at least one shiny gold bag?)
*/

// How many bag colors can eventually contain at least one shiny gold bag
void problem_1::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	Bag_types.emplace(SHINY_GOLD, SHINY_GOLD_TYPE);
	std::vector<bag> bags;
	while (!input.eof()) {
		std::string input_line;
		std::getline(input, input_line);
		bags.emplace_back(input_line);
	}
	input.close();
	
	// Sort the bags by their bag type to make the gold bag first
	std::sort(bags.begin(), bags.end(), bag_lt);

	int bags_have_gold = 0;
	for (bag& cur_bag : bags) {
		if (can_bag_have_gold(cur_bag, bags)) {
			++bags_have_gold;
		}
	}

	std::string answer;
	answer = std::to_string(bags_have_gold);
	output_answer(answer);
}

// How many individual bags are required inside your single shiny gold bag
void problem_2::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	Bag_types.emplace(SHINY_GOLD, SHINY_GOLD_TYPE);
	std::vector<bag> bags;
	while (!input.eof()) {
		std::string input_line;
		std::getline(input, input_line);
		bags.emplace_back(input_line);
	}
	input.close();

	// Sort the bags by their bag type to make the gold bag first
	std::sort(bags.begin(), bags.end(), bag_lt);

	int bags_inside = get_total_bags_inside(bags[SHINY_GOLD_TYPE], bags);

	std::string answer;
	answer = std::to_string(bags_inside);
	output_answer(answer);
}