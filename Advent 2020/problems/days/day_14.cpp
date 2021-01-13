#include "day_14.h"

#include "../common_includes.h"

#include <map>

PROBLEM_CLASS_CPP(14);

enum class operations {
	MASK,
	MEMORY,
};
operations string_to_operation(const std::string& operation_string)
{
	if (operation_string.compare("mask") == 0) {
		return operations::MASK;
	}
	return operations::MEMORY;
}

typedef std::map<long long int, long long int> memory_map;
typedef std::pair<long long int, long long int> masks;   // 1's that get OR'd and 0's that get AND'd
typedef void(*operation_func) (memory_map*, masks*, std::istringstream*);

void mask_func(memory_map* memory_p, masks* masks_p, std::istringstream* input_stream)
{
	input_stream->ignore(64, ' ');
	std::string new_mask;
	std::getline(*input_stream, new_mask);

	masks_p->first = 0;
	masks_p->second = 0;
	int mask_len = new_mask.size() - 1;

	for (int i = mask_len; i >= 0; --i) {
		long long int* mask_p = nullptr;
		if (new_mask[i] == '1') {
			mask_p = &(masks_p->first);
		}
		else if (new_mask[i] == '0') {
			mask_p = &(masks_p->second);
		}
		else {
			continue;
		}

		*mask_p |= (static_cast<long long int>(1) << (mask_len - i));
	}

	masks_p->second = ~masks_p->second; // Not it to be able to and it for the zeros
}

void memory_func(memory_map* memory_p, masks* masks_p, std::istringstream* input_stream)
{
	input_stream->ignore(8, '[');
	long long int address;
	*input_stream >> address;
	if (input_stream->bad() || input_stream->fail()) {
		return;
	}

	input_stream->ignore(4);
	long long int val;
	*input_stream >> val;
	if (input_stream->bad() || input_stream->fail()) {
		return;
	}
	(*memory_p)[address] = (val | masks_p->first) & masks_p->second;
}

void mask_func_2(memory_map* memory_p, masks* masks_p, std::istringstream* input_stream)
{
	input_stream->ignore(64, ' ');
	std::string new_mask;
	std::getline(*input_stream, new_mask);

	masks_p->first = 0;
	masks_p->second = 0;
	int mask_len = new_mask.size() - 1;

	for (int i = mask_len; i >= 0; --i) {
		long long int* mask_p = nullptr;
		if (new_mask[i] == '1') {
			mask_p = &(masks_p->first);
		}
		else if (new_mask[i] == 'X') {
			mask_p = &(masks_p->second);
		}
		else {
			continue;
		}

		*mask_p |= (static_cast<long long int>(1) << (mask_len - i));
	}
	//masks_p->second = ~masks_p->second;
}

void memory_func_2_rec(memory_map* memory_p, long long int cur_val, int cur_mask_ind, masks& masks_p, long long int base_addr, long long int val)
{
	long long int cur_mask = masks_p.second >> (++cur_mask_ind);
	if (cur_mask == 0) {
		if (cur_val == 0) {
			(*memory_p)[base_addr] = val;
		}
		return;
	}

	while ((cur_mask & 1) == 0) {
		cur_mask = cur_mask >> 1;
		++cur_mask_ind;
	}
	cur_mask = static_cast<long long int>(1) << cur_mask_ind;
	(*memory_p)[base_addr + cur_mask + cur_val] = val;

	memory_func_2_rec(memory_p, cur_val, cur_mask_ind, masks_p, base_addr, val);
	memory_func_2_rec(memory_p, cur_val + cur_mask, cur_mask_ind, masks_p, base_addr, val);
}

void memory_func_2(memory_map* memory_p, masks* masks_p, std::istringstream* input_stream)
{
	input_stream->ignore(8, '[');
	long long int address;
	*input_stream >> address;
	if (input_stream->bad() || input_stream->fail()) {
		return;
	}

	input_stream->ignore(4);
	long long int val;
	*input_stream >> val;
	if (input_stream->bad() || input_stream->fail()) {
		return;
	}

	address |= masks_p->first;
	address &= ~masks_p->second;

	memory_func_2_rec(memory_p, 0, -1, *masks_p, address, val);
}

std::map<operations, operation_func> operation_functions = {
	{operations::MASK, mask_func},
	{operations::MEMORY, memory_func}
};

void problem_1::solve(const std::string& file_name)
{
	std::ifstream input_file(file_name);

	memory_map memory;

	masks cur_masks = std::make_pair(0, ~0);
	while (!input_file.eof()) {
		std::string input_line;
		std::getline(input_file, input_line);

		std::istringstream input_line_stream(input_line);
		std::string operation_s;
		std::getline(input_line_stream, operation_s, ' ');

		operations cur_op = string_to_operation(operation_s);
		if (cur_op == operations::MEMORY) {
			input_line_stream.seekg(0, std::ios_base::beg);
		}
		operation_functions[cur_op](&memory, &cur_masks, &input_line_stream);
	}

	input_file.close();

	long long int total = 0;
	for (const auto& memory_addr : memory) {
		total += memory_addr.second;
	}

	std::string answer;
	answer = std::to_string(total);
	output_answer(answer);
}

void problem_2::solve(const std::string& file_name)
{
	operation_functions[operations::MASK] = mask_func_2;
	operation_functions[operations::MEMORY] = memory_func_2;

	std::ifstream input_file(file_name);

	memory_map memory;

	masks cur_masks = std::make_pair(0, ~0);
	while (!input_file.eof()) {
		std::string input_line;
		std::getline(input_file, input_line);

		std::istringstream input_line_stream(input_line);
		std::string operation_s;
		std::getline(input_line_stream, operation_s, ' ');

		operations cur_op = string_to_operation(operation_s);
		if (cur_op == operations::MEMORY) {
			input_line_stream.seekg(0, std::ios_base::beg);
		}
		operation_functions[cur_op](&memory, &cur_masks, &input_line_stream);
	}

	input_file.close();

	long long int total = 0;
	for (const auto& memory_addr : memory) {
		total += memory_addr.second;
	}

	std::string answer;
	answer = std::to_string(total);
	output_answer(answer);
}