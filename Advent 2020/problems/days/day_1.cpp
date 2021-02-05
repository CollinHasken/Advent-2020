#include "day_1.h"

#include "../common_includes.h"

#include <set>

PROBLEM_CLASS_CPP(1);

// Find the two entries that sum to 2020
void problem_1::solve(const std::string& file_name)
{
	static constexpr int GOAL = 2020;

	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	std::set<int> expenses;

	// Get all the expenses
	while (!input.eof()) {
		int new_expense;
		input >> new_expense;
		// If it's greater than the goal, it can't sum to it
		if (new_expense > GOAL) {
			continue;
		}
		expenses.insert(new_expense);
	}
	input.close();

	auto left_iter = expenses.begin();
	auto right_iter = expenses.rbegin();

	// Search the sorted set
	while(*left_iter != *right_iter) {
		// Found the entries
		if (*left_iter + *right_iter == GOAL) {
			break;
		}
		if (*left_iter + *right_iter < GOAL) {
			left_iter++;
		} else {
			right_iter++;
		}
	}

	std::string answer;
	answer = std::to_string(*left_iter * *right_iter);
	output_answer(answer);
}

// What is the product of the three entries that sum to 2020 ?
void problem_2::solve(const std::string& file_name)
{
	static constexpr int GOAL = 2020;

	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	std::set<int> expenses;

	// Get all the expenses
	while (!input.eof()) {
		int new_expense;
		input >> new_expense;
		// If it's greater than the goal, it can't sum to it
		if (new_expense > GOAL) {
			continue;
		}
		expenses.insert(new_expense);
	}
	input.close();
		
	int answer = -1;
	
	// Try each expense in reverse order as the greatest of the 3
	auto left_iter = expenses.begin();
	for(auto right_iter = expenses.rbegin(); *(std::next(right_iter, 1)) != *left_iter; right_iter++){
		left_iter = expenses.begin();
		// Try all other expenses less than the right 
		for (auto mid_iter = std::next(right_iter, 1); *left_iter != *mid_iter; ) {
			// Found answer
			if (*left_iter + *mid_iter + *right_iter == GOAL) {
				answer = *left_iter * *mid_iter * *right_iter;
				break;
			}
			if (*left_iter + *mid_iter + *right_iter < GOAL) {
				left_iter++;
			} else {
				mid_iter++;
			}
		}
		if (answer != -1) {
			break;
		}
	}

	output_answer(std::to_string(answer));
}