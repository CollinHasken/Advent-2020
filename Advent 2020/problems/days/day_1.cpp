#include "day_1.h"

#include "../common_includes.h"

#include <set>

PROBLEM_CLASS_CPP(1);

void problem_1::solve(const std::string& file_name)
{
	static constexpr int GOAL = 2020;

	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	std::set<int> expenses;

	while (!input.eof()) {
		int new_expense;
		input >> new_expense;
		if (new_expense > GOAL) {
			continue;
		}
		expenses.insert(new_expense);
	}

	auto left_iter = expenses.begin();
	auto right_iter = expenses.rbegin();

	for (; *left_iter != *right_iter; ) {
		if (*left_iter + *right_iter == GOAL) {
			break;
		}
		if (*left_iter + *right_iter < GOAL) {
			left_iter++;
		} else {
			right_iter++;
		}
	}

	if (*left_iter == *right_iter) {
		std::cout << "BROKE";
	} else {
		std::cout <<  *left_iter * *right_iter;	
	}

	input.close();
}

void problem_2::solve(const std::string& file_name)
{
	static constexpr int GOAL = 2020;

	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	std::set<int> expenses;

	while (!input.eof()) {
		int new_expense;
		input >> new_expense;
		if (new_expense > GOAL) {
			continue;
		}
		expenses.insert(new_expense);
	}
		
	int answer = -1;
	
	auto left_iter = expenses.begin();
	for(auto ultra_right_iter = expenses.rbegin(); *(std::next(ultra_right_iter, 1)) != *left_iter; ultra_right_iter++){
		left_iter = expenses.begin();
		for (auto right_iter = std::next(ultra_right_iter, 1); *left_iter != *right_iter; ) {
			if (*left_iter + *right_iter + *ultra_right_iter == GOAL) {
				answer = *left_iter * *right_iter * *ultra_right_iter;
				break;
			}
			if (*left_iter + *right_iter + *ultra_right_iter < GOAL) {
				left_iter++;
			} else {
				right_iter++;
			}
		}
		if (answer != -1) {
			break;
		}
	}
	
	std::cout << "Answer: " << answer;

	input.close();
}