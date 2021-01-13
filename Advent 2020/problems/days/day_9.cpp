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

static void add_number_to_preamble(int* index, float target, float* preamble, int preamble_size)
{
	Current_preamble_sorted.erase(preamble[*index]);
	Current_preamble_sorted.emplace(target);
	preamble[*index] = target;
	*index = (*index + 1) % preamble_size;
}

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

static bool add_number_to_sum(std::vector<float>* nums, float num , float target)
{
	float new_sum = std::accumulate(nums->begin(), nums->end(), 0.0f);
	std::size_t nums_to_erase = 0;
	while (nums_to_erase < nums->size() && new_sum + num > target) {
		new_sum -= ((*nums)[nums_to_erase++]);
	}
	if (nums_to_erase != 0) {
		nums->erase(nums->begin(), std::next(nums->begin(), nums_to_erase));
	}

	nums->emplace_back(num);
	new_sum += num;

	return new_sum == target;
}

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

static void common_destruct()
{
	delete[] Current_preamble;
}

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

	std::vector<float> sum;
	while (!input.eof()) {
		std::string input_line;
		std::getline(input, input_line);
		float num = std::stof(input_line);

		if (add_number_to_sum(&sum, num, target)) {
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

	float ans = lower + upper;

	std::string answer;
	answer = std::to_string(ans);
	output_answer(answer);
}