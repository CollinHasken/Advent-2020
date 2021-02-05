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

/*
* This airline uses binary space partitioning to seat people. 
* A seat might be specified like FBFBBFFRLR, where F means "front", B means "back", L means "left", and R means "right".
* Every seat also has a unique seat ID: multiply the row by 8, then add the column
*/

// What is the highest seat ID on a boarding pass?
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
		
		// Create the binary representation for the ticket
		for (int i = 0; i < PASS_NUM_DIGS; ++i) {
			if ((i < PASS_NUM_ROW_DIGS && boarding_pass[i] == PASS_BACK_ROW_CHAR) || (i >= PASS_NUM_ROW_DIGS && boarding_pass[i] == PASS_RIGHT_COL_CHAR)) {
				id |= mask;
			}
			mask = mask >> 1;
		}
		// Save the highest
		if (id > highest_id) {
			highest_id = id;
		}
	}
	input.close();

	output_answer(std::to_string(highest_id));
}

/*
* It's a completely full flight, so your seat should be the only missing boarding pass in your list.
* However, there's a catch: some of the seats at the very front and back of the plane don't exist on this aircraft, so they'll be missing from your list as well.
*/

// What is the ID of your seat
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
		// Create the binary representation for the ticket
		for (int i = 0; i < PASS_NUM_DIGS; ++i) {
			if ((i < PASS_NUM_ROW_DIGS && boarding_pass[i] == PASS_BACK_ROW_CHAR) || (i >= PASS_NUM_ROW_DIGS && boarding_pass[i] == PASS_RIGHT_COL_CHAR)) {
				id |= mask;
			}
			mask = mask >> 1;
		}
		// Save the highest, lowest and total ids
		if (id > highest_id) {
			highest_id = id;
		} 
		if (id < lowest_id) {
			lowest_id = id;
		}
		total_id += id;
	}
	input.close();

	// Compute what the summation from the lowest to highest id should be
	unsigned int theoretical_total = static_cast<unsigned int>((lowest_id + highest_id) * ((highest_id - lowest_id + 1) / 2.f));

	// The difference is the missing id
	unsigned int missing_id = theoretical_total - total_id;

	output_answer(std::to_string(missing_id));
}