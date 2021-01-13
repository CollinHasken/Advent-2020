#include "day_2.h"

#include "../common_includes.h"

PROBLEM_CLASS_CPP(2);
void problem_1::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	int correct_passwords = 0;

	while (!input.eof()) {
		std::string input_line;
		std::getline(input, input_line);

		int min = 0;
		int max = 0;
		char req = 'a';
		char skip = ' ';
		std::string password;
		
		std::istringstream input_line_stream(input_line);

		if (!(input_line_stream >> min >> skip >> max >> req >> skip >> password)) {
			continue;
		}

		int occurances = std::count(password.begin(), password.end(), req);
		if (occurances >= min && occurances <= max) {
			++correct_passwords;
		}
	}

	input.close();

	std::cout << correct_passwords;
}

void problem_2::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	int correct_passwords = 0;

	while (!input.eof()) {
		std::string input_line;
		std::getline(input, input_line);

		int pos1 = 0;
		int pos2 = 0;
		char req = 'a';
		char skip = ' ';
		std::string password;

		std::istringstream input_line_stream(input_line);

		if (!(input_line_stream >> pos1 >> skip >> pos2 >> req >> skip >> password)) {
			continue;
		}

		if ((password[pos1 - 1] == req) ^ (password[pos2 - 1] == req)) {
			++correct_passwords;
		}
	}

	input.close();

	std::cout << correct_passwords;
}