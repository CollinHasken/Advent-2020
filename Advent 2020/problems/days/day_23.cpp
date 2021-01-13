#include "day_23.h"

#include "../common_includes.h"

PROBLEM_CLASS_CPP(23);
//
//class cup {
//public:
//	cup(int label) : m_label(label) {};
//
//	cup* get_prev_cup() const { return m_prev_cup; }
//	cup* get_next_cup() const { return m_next_cup; }
//	void set_next_cup(cup* next_cup) { m_next_cup = next_cup; }
//	void set_prev_cup(cup* next_cup) { m_prev_cup = next_cup; }
//	int get_label() const { return m_label; }
//
//private:
//	int m_label;
//	cup* m_prev_cup = nullptr;
//	cup* m_next_cup = nullptr;
//};
//
//// Delete all cups connected to this cup
//void delete_cups(cup* start_cup)
//{
//	// Break the circular link
//	start_cup->get_prev_cup()->set_next_cup(nullptr);
//
//	// Delete each cup through the list
//	cup* current_cup = start_cup;
//	cup* next_cup = nullptr;
//	while (current_cup != nullptr) {
//		next_cup = current_cup->get_next_cup();
//		delete current_cup;
//		current_cup = next_cup;
//	}
//}
//
//// Find the cup with the label
//const cup* find_cup(int label, const cup* start_cup, bool forward)
//{
//	if (forward) {
//		while (start_cup->get_label() != label) {
//			start_cup = start_cup->get_next_cup();
//		}
//	} else {
//		while (start_cup->get_label() != label) {
//			start_cup = start_cup->get_prev_cup();
//		}
//	}
//	return start_cup;
//}
//
//// Find the cup with the label
//cup* find_cup(int label, cup* start_cup, bool forward)
//{
//	return const_cast<cup*>(find_cup(label, const_cast<const cup*>(start_cup), forward));
//}
//
//bool is_cup_within(int label, cup* start_cup, size_t amount_of_cups)
//{
//	for (size_t cur_cup = 0; cur_cup < amount_of_cups; ++cur_cup) {
//		start_cup = start_cup->get_next_cup();
//		if (start_cup->get_label() == label) {
//			return true;
//		}
//	}
//	return false;
//}
//
//// Get the string produced from the right circular link of the cups starting from the cup labeled 1
//void get_cups_string(std::string* cup_string, const cup* start_cup)
//{
//	const cup* cur_cup = find_cup(1, start_cup, true);
//	cup_string->clear();
//	cur_cup = cur_cup->get_next_cup();
//	while (cur_cup->get_label() != 1) {
//		cup_string->append(std::to_string(cur_cup->get_label()));
//		cur_cup = cur_cup->get_next_cup();
//	}
//}
//
//// Do the move
//void do_move(cup** current_cups, int max_label)
//{
//	int next_cup_id = (*current_cups)->get_label();
//	next_cup_id = next_cup_id == 1 ? max_label : next_cup_id - 1;
//
//	while (is_cup_within(next_cup_id, *current_cups, 3)) {
//		next_cup_id = next_cup_id == 1 ? max_label : next_cup_id - 1;
//	}
//
//	cup* next_cup = find_cup(next_cup_id, *current_cups, false);
//	cup* old_next_cup_next = next_cup->get_next_cup();
//	cup* cups_remove_end = (*current_cups)->get_next_cup()->get_next_cup()->get_next_cup();
//
//	// Link next start with the 3 removed cups
//	next_cup->set_next_cup((*current_cups)->get_next_cup());
//	next_cup->get_next_cup()->set_prev_cup(next_cup);
//
//	// Link the current start cup to the end of the 3 removed cups
//	(*current_cups)->set_next_cup(cups_remove_end->get_next_cup());
//	(*current_cups)->get_next_cup()->set_prev_cup(*current_cups);
//
//	// Link the end of the 3 removed cups to their new next cup
//	cups_remove_end->set_next_cup(old_next_cup_next);
//	old_next_cup_next->set_prev_cup(cups_remove_end);
//
//	*current_cups = (*current_cups)->get_next_cup();
//}
//
//// Print the current state of the cups
//void print_cups(cup* current_cup)
//{
//	int start_label = current_cup->get_label();
//	std::cout << start_label << ' ';
//	current_cup = current_cup->get_next_cup();
//	while (current_cup->get_label() != start_label) {
//		std::cout << current_cup->get_label() << ' ';
//		current_cup = current_cup->get_next_cup();
//	}
//	std::cout << '\n';
//}

#include <unordered_map>
typedef unsigned int label;

typedef std::pair<const label, void*> cup;
typedef std::unordered_map<label, void*> cup_order;

inline cup* get_next_cup(cup* cur_cup) {
	return static_cast<cup*>(cur_cup->second);
}

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


void do_move(cup_order* cups, cup** current_cup, const label max_label)
{
	int next_cup_id = (*current_cup)->first;
	next_cup_id = next_cup_id == 1 ? max_label : next_cup_id - 1;

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

void problem_1::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

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

	cup* current_cup = head_cup;
	const int max_moves = 100;
	const int max_cup_id = 9;
	for (int i = 0; i < max_moves; ++i) {
		do_move(&cups, &current_cup, max_cup_id);
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

	int id = 9;
	const int max_cup_id = 1000000;
	while (id++ < max_cup_id) {
		auto added_cup = cups.emplace(id, nullptr);
		prev_cup->second = static_cast<void*>(&*(added_cup.first));
		prev_cup = &*(added_cup.first);
	}
	(static_cast<cup *>(prev_cup))->second = head_cup;

	cup* current_cup = head_cup;
	const int max_moves = 10000000;
	for (int i = 0; i < max_moves; ++i) {
		do_move(&cups, &current_cup, max_cup_id);
	}

	cup* cup_1 = &*(cups.find(1));
	unsigned long long answer = static_cast<unsigned long long>(get_next_cup(cup_1)->first) * get_next_cup(get_next_cup(cup_1))->first;
	output_answer(std::to_string(answer));
}