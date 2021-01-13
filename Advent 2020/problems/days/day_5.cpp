#include "day_5.h"

#include "../common_includes.h"

PROBLEM_CLASS_CPP(5);

static constexpr int PASS_NUM_ROW_DIGS		= 7;
static constexpr int PASS_NUM_COL_DIGS		= 3;
static constexpr int PASS_NUM_DIGS			= PASS_NUM_ROW_DIGS + PASS_NUM_COL_DIGS;
static constexpr char PASS_BACK_ROW_CHAR	= 'B';
static constexpr char PASS_FRONT_ROW_CHAR	= 'F';
static constexpr char PASS_LEFT_COL_CHAR	= 'L';
static constexpr char PASS_RIGHT_COL_CHAR = 'R';

void problem_1::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	unsigned int highest_id = 0;
	while (!input.eof()) {
		std::string boarding_pass;
		std::getline(input, boarding_pass);

		unsigned int id = 0;
		unsigned int mask = 1 << (PASS_NUM_DIGS - 1);
		for (int i = 0; i < PASS_NUM_DIGS; ++i) {
			if ((i < PASS_NUM_ROW_DIGS && boarding_pass[i] == PASS_BACK_ROW_CHAR) || (i >= PASS_NUM_ROW_DIGS && boarding_pass[i] == PASS_RIGHT_COL_CHAR)) {
				id |= mask;
			}
			mask = mask >> 1;
		}
		if (id > highest_id) {
			highest_id = id;
		}
	}

	input.close();

	output_answer(std::to_string(highest_id));
}

void problem_2::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	unsigned int highest_id = 0;
	unsigned int lowest_id = 1024;
	unsigned int total_id = 0;
	while (!input.eof()) {
		std::string boarding_pass;
		std::getline(input, boarding_pass);

		unsigned int id = 0;
		unsigned int mask = 1 << (PASS_NUM_DIGS - 1);
		for (int i = 0; i < PASS_NUM_DIGS; ++i) {
			if ((i < PASS_NUM_ROW_DIGS && boarding_pass[i] == PASS_BACK_ROW_CHAR) || (i >= PASS_NUM_ROW_DIGS && boarding_pass[i] == PASS_RIGHT_COL_CHAR)) {
				id |= mask;
			}
			mask = mask >> 1;
		}
		if (id > highest_id) {
			highest_id = id;
		} 
		if (id < lowest_id) {
			lowest_id = id;
		}
		total_id += id;
	}

	unsigned int theoretical_total = static_cast<unsigned int>((lowest_id + highest_id) * ((highest_id - lowest_id + 1) / 2.f));
	unsigned int missing_id = theoretical_total - total_id;

	input.close();

	output_answer(std::to_string(missing_id));
}