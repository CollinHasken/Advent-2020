#include "day_10.h"

#include "../common_includes.h"

#include <vector>
#include <algorithm>

PROBLEM_CLASS_CPP(10);

// Store the adaptor voltage and the combinations that can be made from that number
typedef std::pair<int, double> adaptor_combo;

// Sort by the adaptor value
static bool adaptor_cmp(const adaptor_combo& lhs, const adaptor_combo& rhs)
{
	return lhs.first < rhs.first;
}

/*
* Each of your joltage adapters is rated for a specific output joltage (your puzzle input).
* Any given adapter can take an input 1, 2, or 3 jolts lower than its rating and still produce its rated output joltage.
* In addition, your device has a built-in joltage adapter rated for 3 jolts higher than the highest-rated adapter in your bag
* use every adapter in your bag at once, what is the distribution of joltage differences between the charging outlet, the adapters, and your device
*/

// What is the number of 1-jolt differences multiplied by the number of 3-jolt differences
void problem_1::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	std::vector<int> adaptors;

	// Always starts at 0
	adaptors.emplace_back(0);

	// Get all the adaptors
	while (!input.eof()) {
		std::string input_line;
		std::getline(input, input_line);
		adaptors.emplace_back(std::stoi(input_line));
	}

	input.close();

	// Sort them into ascending
	std::sort(adaptors.begin(), adaptors.end());

	// Always ends 3 off
	adaptors.emplace_back(adaptors.back() + 3);

	// Count the amount of jolt jumps
	int one_jolts = 0;
	int three_jolts = 0;
	auto next_iter = std::next(adaptors.cbegin(), 1);
	for (auto iter = adaptors.cbegin(); next_iter != adaptors.cend();) {
		switch ((*next_iter) - (*iter))
		{
		case 1:
			++one_jolts;
			break;
		case 3:
			++three_jolts;
			break;
		default:
			break;
		}
		iter = next_iter;
		++next_iter;
	}

	std::string answer;
	answer = std::to_string(one_jolts * three_jolts);
	output_answer(answer); 
}

/*
* You'll need to figure out how many different ways they can be arranged.
* Every arrangement needs to connect the charging outlet to your device
*/

// What is the total number of distinct ways you can arrange the adapters to connect the charging outlet to your device
void problem_2::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	std::vector<adaptor_combo> adaptors;

	// Always starts at 0
	adaptors.emplace_back(adaptor_combo(0, 0));

	// Get all the adaptors
	while (!input.eof()) {
		std::string input_line;
		std::getline(input, input_line);
		adaptors.emplace_back(adaptor_combo(std::stoi(input_line), 0));
	}

	input.close();

	// Sort them into ascending
	std::sort(adaptors.begin(), adaptors.end(), adaptor_cmp);

	// Always ends 3 off and has 1 combination
	adaptors.emplace_back(adaptor_combo(adaptors.back().first + 3, 1));

	// Last adaptor always has 1 combination
	adaptors[adaptors.size() - 1].second = 1;

	// Go backwards through the adaptors counting how many combinations can be made
	for (int i = adaptors.size() - 2; i > 0; --i) {
		switch (adaptors[i + 2].first - adaptors[i].first)
		{
		case 2:
			// If there's a difference of 2, then we might be able to skip the next 2 adaptors
			if (adaptors[i + 3].first - adaptors[i].first == 3) {
				// Add the combinations we can make with the third number
				adaptors[i].second = adaptors[i + 3].second;
			}
		case 1:
		case 3:
			// If we can skip the next adaptor, then add together the combinations of having it and not having it
			adaptors[i].second += adaptors[i + 1].second + adaptors[i + 2].second;
			break;
		default:
			adaptors[i].second = adaptors[i + 1].second;
			break;
		}		
	}

	std::string answer;
	answer = std::to_string(adaptors.front().second);
	output_answer(answer);
}