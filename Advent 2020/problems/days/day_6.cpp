#include "day_6.h"

#include "../common_includes.h"

#include <algorithm>

PROBLEM_CLASS_CPP(6);

static constexpr int QUESTIONS_MAX = 26;
static constexpr char QUESTION_MIN = 'a';
static constexpr int question_char_to_index(const char question_char) { return static_cast<int>(question_char - QUESTION_MIN); }

class customs_individual {
public:
	customs_individual(const std::string& input);

	bool m_answers[QUESTIONS_MAX] = { false };
};

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

customs_individual::customs_individual(const std::string& input)
{
	for (const char answer : input) {
		m_answers[question_char_to_index(answer)] = true;
	}
}

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
		if (!inited) {
			init_all_answers(ind);
			inited = true;
		}
		add_individual_answers(ind);		
	}
}

void customs_group::add_individual_answers(const customs_individual& ind)
{
	for (int i = 0; i < QUESTIONS_MAX; ++i) {
		if (ind.m_answers[i]) {
			m_answer_present[i] = true;
		} else {
			m_answers_all_present[i] = false;
		}
	}
}

void customs_group::init_all_answers(const customs_individual& ind)
{
	for (int i = 0; i < QUESTIONS_MAX; ++i) {
		if (ind.m_answers[i]) {
			m_answers_all_present[i] = true;
		}
	}
}

int customs_group::get_num_answers_present()
{
	int present = 0;
	for (bool answer : m_answer_present) {
		if (answer) ++present;
	}
	return present;
}

int customs_group::get_num_all_answers_present()
{
	int present = 0;
	for (bool answer : m_answers_all_present) {
		if (answer) ++present;
	}
	return present;
}

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