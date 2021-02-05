#include "day_19.h"

#include "../common_includes.h"

#include <map>
#include <vector>

PROBLEM_CLASS_CPP(19);

// Hash map to rule
// Rule has vector of rules and characters it takes
typedef unsigned int rule_num;
typedef std::vector<rule_num> sub_rule;
class rule;
typedef std::map<rule_num, rule*> rules;

// A rule number and what it requires
class rule {
public:
	virtual ~rule() {};

	rule(rule_num num, unsigned int chars_required) : m_rule_num(num), m_chars_required(chars_required) {};
	bool is_looping() const { return m_is_looping; }
	void set_looping(bool is_looping) { m_is_looping = is_looping; }

	virtual bool has_looping_rule(const rules&) { return is_looping(); }
	virtual unsigned int get_chars_required(const rules&) { return m_chars_required; }
	virtual bool does_message_match(const rules& cur_rules, const std::string& input);

protected:
	rule_num m_rule_num;
	bool m_is_looping = false;
	unsigned int m_chars_required;
};

// A rule with multiple rules within
class rule_of_rules : public rule {
public:
	rule_of_rules(rule_num num) : rule(num, 0) {};
	void add_sub_rule(const sub_rule& new_sub_rule, bool sub_rule_loops) { if (!sub_rule_loops) { m_rules.emplace_back(new_sub_rule); } else { m_rules.emplace(m_rules.begin(), new_sub_rule); } }

	bool has_looping_rule(const rules& cur_rules) override;
	unsigned int get_chars_required(const rules& cur_rules) override;
	bool does_message_match(const rules& cur_rules, const std::string& input) override;
private:
	std::vector<sub_rule> m_rules;
	bool m_has_loop = false;
	bool m_has_checked_for_loop = false;
};

// The base rule that is just the character
class rule_char : public rule {
public:
	rule_char(rule_num num, char rule) : rule(num, 1), m_rule(rule) {};

	bool does_message_match(const rules& cur_rules, const std::string& input) override { return rule::does_message_match(cur_rules, input) && input[0] == m_rule; }
private:
	char m_rule;
};

// Does the message match this rule
//
// cur_rules:	The other current rules
// input:		The input to read
bool rule::does_message_match(const rules& cur_rules, const std::string& input)
{
	// For a base rule, we can only check if this one loops or the sizes match
	return has_looping_rule(cur_rules) || input.size() == get_chars_required(cur_rules);
}

// Does this rule have a looping rule
//
// cur_rules:	The other current rules
bool rule_of_rules::has_looping_rule(const rules& cur_rules) 
{ 
	// Already cached
	if (m_has_checked_for_loop) {
		return m_has_loop;
	}

	m_has_checked_for_loop = true;
	// Check if the base rule knows if this has a looping rule
	// cur_rules is unused
	m_has_loop = rule::has_looping_rule(cur_rules);

	// Need to do deep dive of rules
	if (!m_has_loop) {
		for (const auto& cur_sub_rule : m_rules) {
			for (const auto& cur_sub_rule_num : cur_sub_rule) {
				if (cur_sub_rule_num == m_rule_num || cur_rules.find(cur_sub_rule_num)->second->is_looping()) {
					m_has_loop = true;
					return m_has_loop;
				}
			}
		}
	}

	return m_has_loop;
}

// Get number of characters required to match this rule
//
// cur_rules:	The other current rules
unsigned int rule_of_rules::get_chars_required(const rules& cur_rules)
{
	// Already cached
	if (m_chars_required != 0) {
		return m_chars_required;
	}

	// Sum sub rule required number
	for (const sub_rule& cur_sub_rule : m_rules) {
		unsigned int required_chars = 0;
		for (const rule_num cur_rule_num : cur_sub_rule) {
			// Recursive, set to 0
			if (cur_rule_num == m_rule_num) {
				required_chars = 0;
				break;
			}
			required_chars += cur_rules.find(cur_rule_num)->second->get_chars_required(cur_rules);
		}
		if (required_chars != 0) {
			m_chars_required = required_chars;
			return m_chars_required;
		}
	}
	return m_chars_required;
}

// Test if the string matches the sub rules
//
// cur_rules:		The other current rules
// cur_sub_rules:	The sub rules to check against
// input:			The message
bool test_sub_rule(const rules& cur_rules, const sub_rule& cur_sub_rules, const std::string& input)
{
	int offset = 0;
	// Test each sub rule
	for (size_t cur_sub_rule_ind = 0; cur_sub_rule_ind < cur_sub_rules.size(); ++cur_sub_rule_ind) {
		auto cur_sub_rule = cur_rules.find(cur_sub_rules[cur_sub_rule_ind])->second;
		// For looping subrules
		if (cur_sub_rule->is_looping()) {
			// Determine the maximum characters we can test with the looping sub rule
			unsigned int chars_required_mult = cur_sub_rule->get_chars_required(cur_rules);
			unsigned int max_chars_to_test = input.size() - offset;
			unsigned int remaining_rules_chars = 0;
			bool remaining_rule_loops = false;
			for (size_t remaining_rule_ind = cur_sub_rule_ind + 1; remaining_rule_ind < cur_sub_rules.size(); ++remaining_rule_ind) {
				rule* remaining_rule = cur_rules.find(cur_sub_rules[remaining_rule_ind])->second;
				remaining_rules_chars += remaining_rule->get_chars_required(cur_rules);
				remaining_rule_loops = remaining_rule->is_looping();
			}
			max_chars_to_test -= remaining_rules_chars;

			// If the remaining rule loops, then we need to test this sub rule with each possible splitting of the input between the rules
			if (remaining_rule_loops) {
				// Get the max size that can split into both
				unsigned int current_test_size = max_chars_to_test - (max_chars_to_test % chars_required_mult); 
				while (current_test_size > 0 && (input.size() - offset - current_test_size) % remaining_rules_chars) {
					current_test_size -= chars_required_mult;
				}

				sub_rule remaining_sub_rules(cur_sub_rules.begin() + cur_sub_rule_ind + 1, cur_sub_rules.end());
				while (current_test_size > 0) {
					// If this input chunk works
					if (cur_sub_rule->does_message_match(cur_rules, input.substr(offset, current_test_size))) {
						// Try the remaining input with the remaining rules
						if (!remaining_sub_rules.empty()) {
							if (test_sub_rule(cur_rules, remaining_sub_rules, input.substr(offset + current_test_size))) {
								// If it works for this one and remaining rules, we're good
								return true;
							}
						}
					}

					// Try again with the next valid split
					current_test_size -= chars_required_mult;
					while (current_test_size > 0 && (input.size() - offset - current_test_size) % remaining_rules_chars) {
						current_test_size -= chars_required_mult;
					}
				}
				return false;
			} else {
				// Characters to test isn't valid
				if (max_chars_to_test % chars_required_mult != 0) {
					return false;
				}

				// If this set isn't valid, then we can't adjust it here and throw back false
				if (!cur_sub_rule->does_message_match(cur_rules, input.substr(offset, max_chars_to_test))) {
					return false;
				}
				// If it is valid, move the offset past the tested input for the remaining, non looping rules
				offset += max_chars_to_test;
			}
		} else {
			if (!cur_sub_rule->does_message_match(cur_rules, input.substr(offset, cur_sub_rule->get_chars_required(cur_rules)))) {
				return false;
			}
			offset += cur_sub_rule->get_chars_required(cur_rules);
		}
	}
	return true;
}

// Test if the message matches the rules
bool rule_of_rules::does_message_match(const rules& cur_rules, const std::string& input)
{
	if (input.empty()) {
		return false;
	}

	// Doesn't match basic rule requirements
	if (!rule::does_message_match(cur_rules, input)) {
		return false;
	}
	
	// If there's a looping rule within
	if (has_looping_rule(cur_rules)) {
		std::vector<unsigned int> looping_rules_chars;
		unsigned int non_looping_rules_chars = 0;
		// Add up looping and non-looping rule chars
		for (auto& cur_rule_num : m_rules.back()) {
			rule* cur_rule = cur_rules.find(cur_rule_num)->second;
			if (cur_rule->is_looping()) {
				looping_rules_chars.emplace_back(cur_rule->get_chars_required(cur_rules));
			} else {
				non_looping_rules_chars += cur_rule->get_chars_required(cur_rules);
			}
		}
		// No looping chars and the size doesn't match the non-looping rules chars then it won't work
		if (looping_rules_chars.empty()) {
			if (input.size() % non_looping_rules_chars != 0) {
				return false;
			}
		} else {
			// See if the input can be split into the two looping sizes
			int test_size = input.size() - looping_rules_chars[0];
			while (test_size > 0 && test_size % looping_rules_chars[1] != 0) {
				test_size -= looping_rules_chars[0];
			}
			if (test_size <= 0) {
				return false;
			}
		}
	}

	// If this is a looping rule
	if (is_looping()) {
		unsigned int chars_required_mult = get_chars_required(cur_rules);
		unsigned int max_chars_to_test = input.size();

		// Only 2 rules for recursive rules
		// If it matches the non-recursive one, then try it
		// Otherwise the input needs to match the recursive one or it doesn't work
		if (max_chars_to_test == chars_required_mult) {
			return test_sub_rule(cur_rules, m_rules[1], input);
		}

		return test_sub_rule(cur_rules, m_rules[0], input);
	}

	// Test each sub rule
	for (const sub_rule& cur_sub_rules : m_rules) {
		if (test_sub_rule(cur_rules, cur_sub_rules, input)) {
			return true;
		}
	}

	return false;
}

// Add rule to the list of current rules
//
// cur_rules:	(Output) The current rules that this will be added to
// input:		The input to read the rule from
static void add_rule_from_input(rules* cur_rules, std::string input)
{
	std::istringstream input_stream(input);

	std::string rule_num_s;
	std::getline(input_stream, rule_num_s, ':');

	rule_num cur_rule_num = std::stoi(rule_num_s);

	input_stream.ignore();
	// If this is a character rule
	if (input_stream.peek() == '\"') {
		input_stream.ignore();
		cur_rules->emplace(cur_rule_num, static_cast<rule*>(new rule_char(cur_rule_num, input_stream.get())));
		return;
	}

	// Otherwise its a rule of rules
	rule_of_rules* new_rule = new rule_of_rules(cur_rule_num);
	sub_rule cur_sub_rule;
	bool cur_sub_rule_loops = false;
	while (!input_stream.eof()) {
		std::string new_rule_s;
		std::getline(input_stream, new_rule_s, ' ');

		if (new_rule_s.empty()) {
			break;
		}

		if (new_rule_s[0] == '|') {	
			// Put looping sub rule in the front
			new_rule->add_sub_rule(cur_sub_rule, cur_sub_rule_loops);
			cur_sub_rule_loops = false;
			cur_sub_rule.clear();
			continue;
		}

		int cur_sub_rule_num = std::stoi(new_rule_s);
		if (cur_sub_rule_num == cur_rule_num) {
			cur_sub_rule_loops = true;
			new_rule->set_looping(true);			
		}
		cur_sub_rule.emplace_back(static_cast<rule_num>(std::stoi(new_rule_s)));
	}
	new_rule->add_sub_rule(cur_sub_rule, cur_sub_rule_loops);

	cur_rules->emplace(cur_rule_num, static_cast<rule*>(new_rule));
}

/*
* They sent you a list of the rules valid messages should obey and a list of received messages they've collected so far
* The rules for valid messages (the top part of your puzzle input) are numbered and build upon each other
* The remaining rules list the sub-rules that must be followed
* Some of the rules have multiple lists of sub-rules separated by a pipe (|). This means that at least one list of sub-rules must match
* Fortunately, there are no loops in the rules, so the list of possible matches will be finite
* The received messages (the bottom part of your puzzle input) need to be checked against the rules so you can determine which are valid and which are corrupted
*/

// How many messages completely match rule 0
void problem_1::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	rules cur_rules;

	// Get rules
	while (!input.eof()) {
		std::string input_line;
		std::getline(input, input_line);
		if (input_line.empty()) {
			break;
		}

		add_rule_from_input(&cur_rules, input_line);
	}

	rule* first_rule = cur_rules.find(0)->second;
	int valid_messages = 0;

	// Test messages
	while (!input.eof()) {
		std::string input_line;
		std::getline(input, input_line);

		if (first_rule->does_message_match(cur_rules, input_line)) {
			++valid_messages;
		}
	}
	input.close();

	std::string answer;
	answer = std::to_string(valid_messages);
	output_answer(answer);
}

/*
* Completely replace rules 8: 42 and 11: 42 31 with the following:
* 8: 42 | 42 8
* 11: 42 31 | 42 11 31
* Now, the rules do contain loops, and the list of messages they could hypothetically match is infinite
* You only need to handle the rules you have
*/

// After updating rules 8 and 11, how many messages completely match rule 0
void problem_2::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	rules cur_rules;

	// Get rules
	while (!input.eof()) {
		std::string input_line;
		std::getline(input, input_line);
		if (input_line.empty()) {
			break;
		}

		add_rule_from_input(&cur_rules, input_line);
	}

	rule* first_rule = cur_rules.find(0)->second;
	int valid_messages = 0;

	// Test messages
	while (!input.eof()) {
		std::string input_line;
		std::getline(input, input_line);

		if (first_rule->does_message_match(cur_rules, input_line)) {
			++valid_messages;
		}
	}

	input.close();

	std::string answer;
	answer = std::to_string(valid_messages);
	output_answer(answer);
}