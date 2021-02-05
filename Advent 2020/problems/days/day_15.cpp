#include "day_15.h"

#include "../common_includes.h"

#include <map>

PROBLEM_CLASS_CPP(15);

/*
* In this game, the players take turns saying numbers.
* They begin by taking turns reading from a list of starting numbers (your puzzle input).
* Then, each turn consists of considering the most recently spoken number:
*   If that was the first time the number has been spoken, the current player says 0.
*   Otherwise, the number had been spoken before; the current player announces how many turns apart the number is from when it was previously spoken.
* So, after the starting numbers, each turn results in that player speaking aloud either 0 (if the last number is new) or an age (if the last number is a repeat)
*/

// What will be the 2020th number spoken
void problem_1::solve(const std::string& file_name)
{
	std::ifstream input_file(file_name);
	// Store each number and what turn it was said
	std::map<unsigned int, unsigned int> said_numbers;
	unsigned int turn = 0;
	unsigned int prev_num = 0;
	// Get the initialy said numbers
	while (!input_file.eof()) {
		std::string input_line;
		std::getline(input_file, input_line);

		std::istringstream input_line_stream(input_line);
		std::string number;
		while (!input_line_stream.eof()) {
			std::getline(input_line_stream, number, ',');
			prev_num = std::stoi(number);
			said_numbers[prev_num] = ++turn;
		}
	}
	input_file.close();

	// Get to turn 2020
	while (turn < 2020) {
		const auto& prev_num_itr = said_numbers.find(prev_num);

		// If the number wasn't found, the new number is 0
		// Otherwise its the difference of the current turn and when it was last said
		int new_num = prev_num_itr != said_numbers.end() ? turn - prev_num_itr->second : 0;

		// Update latest turn this word was said
		said_numbers[prev_num] = turn++;
		prev_num = new_num;
	}

	std::string answer;
	answer = std::to_string(prev_num);
	output_answer(answer);
}

// What will be the 30000000th number spoken
void problem_2::solve(const std::string& file_name)
{
	std::ifstream input_file(file_name);
	// Store each number and what turn it was said
	std::map<unsigned int, unsigned int> said_numbers;
	unsigned int turn = 0;
	unsigned int prev_num = 0;
	// Get the initialy said numbers
	while (!input_file.eof()) {
		std::string input_line;
		std::getline(input_file, input_line);

		std::istringstream input_line_stream(input_line);
		std::string number;
		while (!input_line_stream.eof()) {
			std::getline(input_line_stream, number, ',');
			prev_num = std::stoi(number);
			said_numbers[prev_num] = ++turn;
		}
	}

	// Get to turn 30000000
	while (turn < 30000000) {
		const auto& prev_num_itr = said_numbers.find(prev_num);

		// If the number wasn't found, the new number is 0
		// Otherwise its the difference of the current turn and when it was last said
		int new_num = prev_num_itr != said_numbers.end() ? turn - prev_num_itr->second : 0;

		// Update latest turn this word was said
		said_numbers[prev_num] = turn++;
		prev_num = new_num;
	}

	input_file.close();

	std::string answer;
	answer = std::to_string(prev_num);
	output_answer(answer);
}