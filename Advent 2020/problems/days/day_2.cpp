#include "day_2.h"

#include "../common_includes.h"

PROBLEM_CLASS_CPP(2);

/*
* Each line gives the password policy and then the password.
* The password policy indicates the lowest and highest number of times
* a given letter must appear for the password to be valid.
* For example, 1-3 a means that the password must contain a at least 1 time and at most 3 times.
*/

// How many passwords are valid according to their policies?
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

		// Extract info
		if (!(input_line_stream >> min >> skip >> max >> req >> skip >> password)) {
			continue;
		}

		// Count occurances and test
		int occurances = std::count(password.begin(), password.end(), req);
		if (occurances >= min && occurances <= max) {
			++correct_passwords;
		}
	}
	input.close();

	output_answer(std::to_string(correct_passwords));
}

/*
* Each policy actually describes two positions in the password, 
* where 1 means the first character, 2 means the second character, and so on.
* (Be careful; Toboggan Corporate Policies have no concept of "index zero"!) 
* Exactly one of these positions must contain the given letter. Other occurrences 
* of the letter are irrelevant for the purposes of policy enforcement.
*/

// How many passwords are valid according to their policies?
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

		// Extract info
		if (!(input_line_stream >> pos1 >> skip >> pos2 >> req >> skip >> password)) {
			continue;
		}

		// Only one position can have the required letter
		if ((password[pos1 - 1] == req) ^ (password[pos2 - 1] == req)) {
			++correct_passwords;
		}
	}
	input.close();

	output_answer(std::to_string(correct_passwords));
}