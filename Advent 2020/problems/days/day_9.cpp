#include "day_9.h"

#include "../common_includes.h"

#include <set>
#include <vector>
#include <limits>
#include <numeric>

#undef max
#undef min

PROBLEM_CLASS_CPP(9);

static int Preamble_length;
static float* Current_preamble;
static int Current_index;
static std::set<float> Current_preamble_sorted;

// Add the number to the preamble array and sorted set
//
// index:			(Output) The index in the array for this new preamble
// target:			The new number
// preamble:		The array to add to
// preamble_size: The size of the array
static void add_number_to_preamble(int* index, float target, float* preamble, int preamble_size)
{
	// Remove the oldest preamble number
	Current_preamble_sorted.erase(preamble[*index]);

	// Add new one
	Current_preamble_sorted.emplace(target);
	preamble[*index] = target;
	*index = (*index + 1) % preamble_size;
}

// Determine if the number is the sum of any two numbers in the preamble
//
// target:				The sum to use
// sorted_preamble:	The preamble to search
static bool is_preamble(float target, std::set<float>& sorted_preamble)
{
	auto lower_iter = sorted_preamble.cbegin();
	if (*lower_iter > target) {
		return false;
	}

	if (*lower_iter + *(std::next(lower_iter, 1)) > target) {
		return false;
	}

	auto upper_iter = sorted_preamble.crbegin();
	if (*upper_iter + *(std::next(upper_iter, 1)) < target) {
		return false;
	}

	// Search for the target sum
	while (*lower_iter != *upper_iter) {
		if (*lower_iter + *upper_iter == target) {
			return true;
		} else if (*lower_iter + *upper_iter < target) {
			++lower_iter;
		} else {
			++upper_iter;
		}
	}

	return false;
}

// Add the given number to the sum. If it goes over, remove the old numbers until we're under
//
// nums:				(Output) Vector of the numbers to sum up
// current_sum:	(Output) The current sum of the numbers
// num:				The new number to add
// target:			The target value
//
// Returns true if the newly added number reaches the target
static bool add_number_to_sum(std::vector<float>* nums, float* current_sum, float num , float target)
{
	// Remove old entries until the new sum isn't over the target
	std::size_t nums_to_erase = 0;
	while (nums_to_erase < nums->size() && *current_sum + num > target) {
		*current_sum -= ((*nums)[nums_to_erase++]);
	}
	if (nums_to_erase != 0) {
		nums->erase(nums->begin(), std::next(nums->begin(), nums_to_erase));
	}

	// Add new number
	nums->emplace_back(num);
	*current_sum += num;

	return *current_sum == target;
}

// Setup global variables
//
// preamble:	Length of numbers for the preamble
// input:		Input to read from
static void common_setup(int preamble, std::ifstream* input)
{
	Current_index = 0;
	Preamble_length = preamble;
	Current_preamble = new float[Preamble_length];
	Current_preamble_sorted.clear();

	// Setup preamble
	for (int i = 0; i < Preamble_length; ++i) {
		std::string input_line;
		std::getline(*input, input_line);
		float target = std::stof(input_line);
		Current_preamble[i] = target;
		Current_preamble_sorted.emplace(target);
	}
}

// Clear global variables
static void common_destruct()
{
	delete[] Current_preamble;
}

/*
* Transmits a preamble of 25 numbers. After that, each number you receive should be the sum 
* of any two of the 25 immediately previous numbers. The two numbers will have different values, and there might be more than one such pair.
*/

//  What is the first number that does not have this property
void problem_1::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	common_setup(25, &input);

	float target = 0;
	while (!input.eof()) {
		std::string input_line;
		std::getline(input, input_line);
		target = std::stof(input_line);

		if (!is_preamble(target, Current_preamble_sorted)) {
			break;
		}		

		add_number_to_preamble(&Current_index, target, Current_preamble, Preamble_length);
	}

	input.close();

	common_destruct();

	std::string answer;
	answer = std::to_string(target);
	output_answer(answer); 
}

/*
* You must find a contiguous set of at least two numbers in your list which sum to the invalid number from step 1
* To find the encryption weakness, add together the smallest and largest number in this contiguous range
*/

// What is the encryption weakness in your XMAS-encrypted list of numbers
void problem_2::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	common_setup(25, &input);

	float target = 0;
	while (!input.eof()) {
		std::string input_line;
		std::getline(input, input_line);
		target = std::stof(input_line);

		if (!is_preamble(target, Current_preamble_sorted)) {
			break;
		}

		add_number_to_preamble(&Current_index, target, Current_preamble, Preamble_length);
	}

	// Reset input
	input.seekg(0, std::ios_base::beg);

	// Add each number to the sum until it equals the invalid number
	std::vector<float> sum;
	float current_sum = 0;
	while (!input.eof()) {
		std::string input_line;
		std::getline(input, input_line);
		float num = std::stof(input_line);

		if (add_number_to_sum(&sum, &current_sum, num, target)) {
			break;
		}
	}

	input.close(); 

	float lower = std::numeric_limits<float>::max();
	float upper = std::numeric_limits<float>::min();
	for (const auto& num : sum) {
		if (num < lower) {
			lower = num;
		}
		if (num > upper) {
			upper = num;
		}
	}

	common_destruct();

	std::string answer;
	answer = std::to_string(lower + upper);
	output_answer(answer);
}