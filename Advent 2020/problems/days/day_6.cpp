#include "day_6.h"

#include "../common_includes.h"

#include <algorithm>

PROBLEM_CLASS_CPP(6);

static constexpr int QUESTIONS_MAX = 26;
static constexpr char QUESTION_MIN = 'a';
static constexpr int question_char_to_index(const char question_char) { return static_cast<int>(question_char - QUESTION_MIN); }

// Class to hold an individual's answers to the form
class customs_individual {
public:
	customs_individual(const std::string& input);

	bool m_answers[QUESTIONS_MAX] = { false };
};

// Class to hold the answers a whole group has
class customs_group {
public:
	customs_group(std::ifstream& input);
	int get_num_answers_present();
	int get_num_all_answers_present();

	bool m_answer_present[QUESTIONS_MAX] = { false };
	bool m_answers_all_present[QUESTIONS_MAX] = { false };

	void add_individual_answers(const customs_individual& ind);
	void init_all_answers(const customs_individual& ind);
};

// Create an individual's customs from input
//
// input:	Character array of answered questions
customs_individual::customs_individual(const std::string& input)
{
	for (const char answer : input) {
		m_answers[question_char_to_index(answer)] = true;
	}
}

// Create a group's customs from input
//
// input:	Individual customs seperated by new lines
customs_group::customs_group(std::ifstream& input)
{
	bool inited = false;
	while (!input.eof()) {
		std::string customs_ind;
		std::getline(input, customs_ind);
		if (customs_ind.empty()) {
			return;
		}

		customs_individual ind(customs_ind);
		// We start by setting the answers to false
		// and the first individual will set their answers to true
		// Then future individuals will set ones they don't have as false
		if (!inited) {
			init_all_answers(ind);
			inited = true;
		}
		add_individual_answers(ind);		
	}
}

// Add the individual's answer's to the group's
//
// ind:	The individual to add
void customs_group::add_individual_answers(const customs_individual& ind)
{
	for (int i = 0; i < QUESTIONS_MAX; ++i) {
		// If it's present, then make sure we mark that at least one person has it
		if (ind.m_answers[i]) {
			m_answer_present[i] = true;
		} else {
			// Mark they don't all have
			m_answers_all_present[i] = false;
		}
	}
}

// Init all the answers
//
// ind:	The individual to set intial answers that are present
void customs_group::init_all_answers(const customs_individual& ind)
{
	for (int i = 0; i < QUESTIONS_MAX; ++i) {
		if (ind.m_answers[i]) {
			m_answers_all_present[i] = true;
		}
	}
}

// Get the number of answers that any member has answered
//
// Returns number of answers
int customs_group::get_num_answers_present()
{
	int present = 0;
	for (bool answer : m_answer_present) {
		if (answer) ++present;
	}
	return present;
}

// Get the number of answers all members have answered
//
// Returns number of answers
int customs_group::get_num_all_answers_present()
{
	int present = 0;
	for (bool answer : m_answers_all_present) {
		if (answer) ++present;
	}
	return present;
}

/*
* The form asks a series of 26 yes-or-no questions marked a through z.
* For each of the people in their group, you write down the questions for which they answer "yes", one per line
* You've collected answers from every group on the plane
*/

// For each group, count the number of questions to which anyone answered "yes"
void problem_1::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	int present_answers = 0;
	while (!input.eof()) {
		customs_group group(input);
		present_answers += group.get_num_answers_present();
	}
	input.close();

	std::string answer;
	answer = std::to_string(present_answers);
	output_answer(answer);
}

/*
* You don't need to identify the questions to which anyone answered "yes"
* You need to identify the questions to which everyone answered "yes"
*/

// For each group, count the number of questions to which everyone answered "yes"
void problem_2::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	int present_answers = 0;
	while (!input.eof()) {
		customs_group group(input);
		present_answers += group.get_num_all_answers_present();
	}

	input.close();

	std::string answer;
	answer = std::to_string(present_answers);
	output_answer(answer);
}