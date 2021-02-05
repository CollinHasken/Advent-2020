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

// Update the mask
//
// memory_p:		(Output) The memory address
// masks_p:			(Output) The two masks
// input_stream:	The input
void mask_func(memory_map* memory_p, masks* masks_p, std::istringstream* input_stream)
{
	input_stream->ignore(64, ' ');
	std::string new_mask;
	std::getline(*input_stream, new_mask);

	masks_p->first = 0;
	masks_p->second = 0;
	int mask_len = new_mask.size() - 1;

	// Get mask from the input
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

	// Not it to be able to and it for the zeros
	masks_p->second = ~masks_p->second; 
}

// Set the memeory
// 
// memory_p:		(Output) The memory address
// masks_p:			(Output) The two masks
// input_stream:	The input
void memory_func(memory_map* memory_p, masks* masks_p, std::istringstream* input_stream)
{
	// Get the address
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
	// Set the value at the address after applying the mask
	(*memory_p)[address] = (val | masks_p->first) & masks_p->second;
}

// Update the mask
//
// memory_p:		(Output) The memory address
// masks_p:			(Output) The two masks
// input_stream:	The input
void mask_func_2(memory_map* memory_p, masks* masks_p, std::istringstream* input_stream)
{
	input_stream->ignore(64, ' ');
	std::string new_mask;
	std::getline(*input_stream, new_mask);

	masks_p->first = 0;
	masks_p->second = 0;
	int mask_len = new_mask.size() - 1;

	// Get mask from the input
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
	// Don't want to not the second mask this time
}

// Recursively set the memory at the address that fit the floating mask bits
//
// memory_p:		(Output) The memory address
// cur_val:			The current address value to add
// cur_mask_ind:	The index we are currently at for the floating bit
// masks_p:			The masks
// base_addr:		The base address we started from
// val:				The value to set at the addresses
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

// Set the memeory
// 
// memory_p:		(Output) The memory address
// masks_p:			(Output) The two masks
// input_stream:	The input
void memory_func_2(memory_map* memory_p, masks* masks_p, std::istringstream* input_stream)
{
	// Get teh address
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

	// Update the address from the mask for the start of recurssion
	address |= masks_p->first;
	address &= ~masks_p->second;

	// Set all memory address that fit the mask
	memory_func_2_rec(memory_p, 0, -1, *masks_p, address, val);
}

std::map<operations, operation_func> operation_functions = {
	{operations::MASK, mask_func},
	{operations::MEMORY, memory_func}
};

/*
* The initialization program (your puzzle input) can either update the bitmask or write a value to memory.
* Values and memory addresses are both 36-bit unsigned integers
* The bitmask is always given as a string of 36 bits, written with the most significant bit (representing 2^35) on the left and the least significant bit (2^0, that is, the 1s bit) on the right.
* The current bitmask is applied to values immediately before they are written to memory: a 0 or 1 overwrites the corresponding bit in the value, while an X leaves the bit in the value unchanged
*/

// What is the sum of all values left in memory after it completes
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
			// Need to read the operation again for the operation
			input_line_stream.seekg(0, std::ios_base::beg);
		}
		operation_functions[cur_op](&memory, &cur_masks, &input_line_stream);
	}
	input_file.close();

	// Total up the memories
	long long int total = 0;
	for (const auto& memory_addr : memory) {
		total += memory_addr.second;
	}

	std::string answer;
	answer = std::to_string(total);
	output_answer(answer);
}

/*
* Immediately before a value is written to memory, each bit in the bitmask modifies the corresponding bit of the destination memory address in the following way
* If the bitmask bit is 0, the corresponding memory address bit is unchanged.
* If the bitmask bit is 1, the corresponding memory address bit is overwritten with 1.
* If the bitmask bit is X, the corresponding memory address bit is floating.
* A floating bit is not connected to anything and instead fluctuates unpredictably.
* In practice, this means the floating bits will take on all possible values, potentially causing many memory addresses to be written all at once
*/

// What is the sum of all values left in memory after it completes
void problem_2::solve(const std::string& file_name)
{
	// Change the functions to the second version
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
			// Need to read the operation again for the operation
			input_line_stream.seekg(0, std::ios_base::beg);
		}
		operation_functions[cur_op](&memory, &cur_masks, &input_line_stream);
	}
	input_file.close();

	// Total up the memories
	long long int total = 0;
	for (const auto& memory_addr : memory) {
		total += memory_addr.second;
	}

	std::string answer;
	answer = std::to_string(total);
	output_answer(answer);
}