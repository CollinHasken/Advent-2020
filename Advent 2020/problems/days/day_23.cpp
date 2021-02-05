#include "day_23.h"

#include "../common_includes.h"

#include <unordered_map>

PROBLEM_CLASS_CPP(23);

typedef unsigned int label;

// Cup and it's next cup
typedef std::pair<const label, void*> cup;
typedef std::unordered_map<label, void*> cup_order;

// Get the next cup from it's cached iter
inline cup* get_next_cup(cup* cur_cup) {
	return static_cast<cup*>(cur_cup->second);
}

// Is the cup within the start cup to its 2 next neighbors
// 
// start_cup:		Cup to start the search
// search_label:	Cup we're searching for
//
// Returns true if it's within
bool is_cup_within(const cup& start_cup, const label search_label)
{
	auto current_cup = static_cast<cup*>(start_cup.second);
	for (size_t i = 0; i < 3; ++i) {
		if (current_cup->first == search_label) {
			return true;
		}
		current_cup = static_cast<cup*>(current_cup->second);
	}
	return false;
}

// Do the move
//
// current_cup:	(Output) The current cup
// cups:				(Output) The cups to work with
// max_label:		The maximum label within these cups
void do_move(cup** current_cup, cup_order* cups, const label max_label)
{
	int next_cup_id = (*current_cup)->first;
	next_cup_id = next_cup_id == 1 ? max_label : next_cup_id - 1;

	// Get the next ID that isn't within the current cup and its 2 next neighbors
	while (is_cup_within(*(*current_cup), next_cup_id)) {
		next_cup_id = next_cup_id == 1 ? max_label : next_cup_id - 1;
	}

	cup* next_cup = &*((*cups).find(next_cup_id));
	cup* end_of_three_cups = get_next_cup(get_next_cup(get_next_cup(*current_cup)));
	void* old_next_cup_next = next_cup->second;

	// Link next start with the 3 removed cups
	next_cup->second = (*current_cup)->second;

	// Link the current start cup to the end of the 3 removed cups
	(*current_cup)->second = end_of_three_cups->second;

	// Link the end of the 3 removed cups to their new next cup
	end_of_three_cups->second = old_next_cup_next;

	*current_cup = get_next_cup(*current_cup);
}

// Print the current state of the cups
void get_cup_string(std::string* output, cup_order* cups)
{
	cup* cur_cup = &*(cups->find(1));
	output->clear();
	cur_cup = get_next_cup(cur_cup);
	while (cur_cup->first != 1) {
		output->append(std::to_string(cur_cup->first));
		cur_cup = get_next_cup(cur_cup);
	}
}

// Print the current state of the cups
void print_cups(cup* current_cup)
{
	label start_label = current_cup->first;
	std::cout << start_label << ' ';
	current_cup = get_next_cup(current_cup);
	while (current_cup->first != start_label) {
		std::cout << current_cup->first << ' ';
		current_cup = get_next_cup(current_cup);
	}
	std::cout << '\n';
}

/*
* The cups will be arranged in a circle and labeled clockwise
* the first cup in your list as the current cup. The crab is then going to do 100 moves.
* Each move, the crab does the following actions:
*   The crab picks up the three cups that are immediately clockwise of the current cup. They are removed from the circle; cup spacing is adjusted as necessary to maintain the circle.
*   The crab selects a destination cup: the cup with a label equal to the current cup's label minus one. 
*     If this would select one of the cups that was just picked up, the crab will keep subtracting one until it finds a cup that wasn't just picked up.
*     If at any point in this process the value goes below the lowest value on any cup's label, it wraps around to the highest value on any cup's label instead.
*   The crab places the cups it just picked up so that they are immediately clockwise of the destination cup. They keep the same order as when they were picked up.
*   The crab selects a new current cup: the cup which is immediately clockwise of the current cup.
* Starting after the cup labeled 1, collect the other cups' labels clockwise into a single string with no extra characters
*/

// What are the labels on the cups after cup 1
void problem_1::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	// Get all the input cups and link them together
	cup* head_cup = nullptr;
	cup* prev_cup = nullptr;
	cup_order cups;
	while (!input.eof()) {
		std::string input_line;
		std::getline(input, input_line);
		for (const char input_char : input_line) {
			auto added_cup = cups.emplace((input_char - '0'), nullptr);
			if (prev_cup != nullptr) {
				prev_cup->second = static_cast<void*>(&*(added_cup.first));
			}
			prev_cup = &*(added_cup.first);
			if (head_cup == nullptr) {
				head_cup = prev_cup;
			}
		}
	}
	input.close();
	if (prev_cup == nullptr) {
		return;
	}

	(static_cast<cup*>(prev_cup))->second = head_cup;

	// Do the moves for the amount of times
	cup* current_cup = head_cup;
	const int max_moves = 100;
	const int max_cup_id = 9;
	for (int i = 0; i < max_moves; ++i) {
		do_move(&current_cup, &cups, max_cup_id);
	}

	std::string answer;
	get_cup_string(&answer, &cups);
	output_answer(answer);
}

void problem_2::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	// Get all the input cups and link them together
	cup* head_cup = nullptr;
	cup* prev_cup = nullptr;
	cup_order cups;
	while (!input.eof()) {
		std::string input_line;
		std::getline(input, input_line);
		for (const char input_char : input_line) {
			auto added_cup = cups.emplace((input_char - '0'), nullptr);
			if (prev_cup != nullptr) {
				prev_cup->second = static_cast<void*>(&*(added_cup.first));
			}
			prev_cup = &*(added_cup.first);
			if (head_cup == nullptr) {
				head_cup = prev_cup;
			} 
		}
	}
	input.close();
	if (prev_cup == nullptr) {
		return;
	}

	// Add the extra cups
	int id = 9;
	const int max_cup_id = 1000000;
	while (id++ < max_cup_id) {
		auto added_cup = cups.emplace(id, nullptr);
		prev_cup->second = static_cast<void*>(&*(added_cup.first));
		prev_cup = &*(added_cup.first);
	}
	(static_cast<cup *>(prev_cup))->second = head_cup;

	// Do the moves for the amount of times
	cup* current_cup = head_cup;
	const int max_moves = 10000000;
	for (int i = 0; i < max_moves; ++i) {
		do_move(&current_cup, &cups, max_cup_id);
	}

	cup* cup_1 = &*(cups.find(1));
	unsigned long long answer = static_cast<unsigned long long>(get_next_cup(cup_1)->first) * get_next_cup(get_next_cup(cup_1))->first;
	output_answer(std::to_string(answer));
}