#include "day_15.h"

#include "../common_includes.h"

#include <map>

PROBLEM_CLASS_CPP(15);

void problem_1::solve(const std::string& file_name)
{
	std::ifstream input_file(file_name);
	std::map<unsigned int, unsigned int> said_numbers;
	unsigned int turn = 0;
	unsigned int prev_num = 0;
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

	while (turn < 2020) {
		const auto& prev_num_itr = said_numbers.find(prev_num);
		int new_num = prev_num_itr != said_numbers.end() ? turn - prev_num_itr->second : 0;
		said_numbers[prev_num] = turn++;
		prev_num = new_num;
	}

	input_file.close();

	std::string answer;
	answer = std::to_string(prev_num);
	output_answer(answer);
}

void problem_2::solve(const std::string& file_name)
{
	std::ifstream input_file(file_name);
	std::map<unsigned int, unsigned int> said_numbers;
	unsigned int turn = 0;
	unsigned int prev_num = 0;
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

	while (turn < 30000000) {
		const auto& prev_num_itr = said_numbers.find(prev_num);
		int new_num = prev_num_itr != said_numbers.end() ? turn - prev_num_itr->second : 0;
		said_numbers[prev_num] = turn++;
		prev_num = new_num;
	}

	input_file.close();

	std::string answer;
	answer = std::to_string(prev_num);
	output_answer(answer);
}