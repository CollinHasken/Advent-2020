#include "day_25.h"

#include "../common_includes.h"

PROBLEM_CLASS_CPP(25);

static constexpr unsigned int LOOP_SUBJECT = 7;
static constexpr unsigned int LOOP_DIVIDOR = 20201227;

int get_loop_size(bool* is_card_loop_size, int card_pk, int door_pk)
{
	int loop_size = 0;
	long long int value = 1;
	while (value != card_pk && value != door_pk) {
		++loop_size;
		//	Set the value to itself multiplied by the subject number
		value *= LOOP_SUBJECT;
		//	Set the value to the remainder after dividing the value by 20201227.
		value %= LOOP_DIVIDOR;
	}
	*is_card_loop_size = value == card_pk;
	return loop_size;
}

int perform_loop(int subject, int loop_size)
{
	long long int value = 1;
	for (int i = 0; i < loop_size; ++i) {
		//	Set the value to itself multiplied by the subject number
		value *= subject;
		//	Set the value to the remainder after dividing the value by 20201227.
		value %= LOOP_DIVIDOR;
	}
	return value;
}

void problem_1::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	std::string input_line;
	std::getline(input, input_line);
	int card_pk = std::stoi(input_line);
	
	std::getline(input, input_line);
	int door_pk = std::stoi(input_line);

	input.close();

	bool is_card_loop_size = false;
	int loop_size = get_loop_size(&is_card_loop_size, card_pk, door_pk);

	int encryption_key = perform_loop(is_card_loop_size ? door_pk : card_pk, loop_size);

	std::string answer;
	answer = std::to_string(encryption_key);
	output_answer(answer);
}

void problem_2::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	while (!input.eof()) {
		std::string input_line;
		std::getline(input, input_line);

	}

	input.close();

	/*std::string answer;
	answer = std::to_string(highest_id);
	output_answer(answer);*/
}