#include "day_8.h"

#include "../common_includes.h"

#include <map>
#include <stack>
#include <limits>

#undef max

PROBLEM_CLASS_CPP(8);

typedef void (*instruction_sig)(int, std::ifstream&, bool);

static constexpr int MAX_LINES = 999;
static constexpr char PLUS = '+';
static const char* const INST_ACC = "acc";
static const char* const INST_JMP = "jmp";
static const char* const INST_NOP = "nop";

static int Accumulator;
static int Current_line;
static bool Visited_lines[MAX_LINES] = { false };
static int Line_flipped;

static std::map<int, instruction_sig> instructions;

// Convert the command code to crc
static int code_to_crc(const char code[4])
{
	int crc = 0;
	for (int i = 0; i < 4; ++i) crc += static_cast<int>(code[i]);
	return crc;
}

// Convert the command code to crc
static int code_to_crc(const std::string& code)
{
	int crc = 0;
	for (const char c : code) crc += static_cast<int>(c);
	return crc;
}

static const int INST_ACC_CRC = code_to_crc(INST_ACC);
static const int INST_JMP_CRC = code_to_crc(INST_JMP);
static const int INST_NOP_CRC = code_to_crc(INST_NOP);

// Frame to record the current state
struct frame {
	frame(int acc, int line, std::streampos pos) : m_acc(acc), m_line(line), m_pos(pos) { };
	int m_acc;
	int m_line;
	std::streampos m_pos;
};

// Frame stack to record the states of the program
static std::stack<frame> Frame_stack;

// Go to the next line of the input stream
static void inline next_line(std::ifstream& input)
{
	input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	++Current_line;
}

// Go to the previous line in the input
static void inline prev_line(std::ifstream& input)
{
	input.seekg(-3,std::ios_base::cur); //\n and last char
	while (input.peek() != '\n') {
		input.seekg(-1, std::ios_base::cur);
	}
	input.ignore();
	--Current_line;
}

// Perform the accumulate operation
// 
// arg:		Amount to add to accumulator
// input:	Remaining input 
// flip:		Unused
static void acc(int arg, std::ifstream& input, bool flip)
{
	if (flip) {
		Accumulator -= arg;
	} else {
		Accumulator += arg;
	}
	next_line(input);
}

static void nop(int arg, std::ifstream& input, bool flip);
// Perform the jump operation to jump to the specified line
//
// arg:		The amount of lines to jump
// input:	Remaining input
// flip:		If we should flip this to a nop
static void jmp(int arg, std::ifstream& input, bool flip)
{
	if (flip) {
		nop(arg, input, false);
	} else {
		if (arg >= 0) {
			for (int i = 0; i < arg; ++i) {
				next_line(input);
			}
		} else {
			for (int i = 0; i > arg; --i) {
				prev_line(input);
			}
		}
	}
}

// Perform the nop operation
//
// arg:		Used only if we flip
// input:	Remaining input
// flip:		Whether to flip this to a jump perform
static void nop(int arg, std::ifstream& input, bool flip)
{
	if (flip) {
		jmp(arg, input, false);
	} else {
		next_line(input);
	}
}

// Get the instruction from the input line
// inst_crc:	(Output) CRC of the operation
// arg:			(Output) Arg for the operation
// input:		Input line to extract from
static void get_instruction(int* inst_crc, int* arg, std::ifstream& input)
{
	std::streampos pos = input.tellg();
	std::string instruction_line;
	std::getline(input, instruction_line);
	// We want to keep the input stream where it was
	input.seekg(pos);

	std::string inst = instruction_line.substr(0, 3);
	bool positive = instruction_line[4] == PLUS;

	*arg = std::stoi(instruction_line.substr(5)) * (positive ? 1 : -1);
	*inst_crc = code_to_crc(inst);
}

// Interpret the next line of the input
// 
// input:	Input to read and update
// flip:		Whether the operation should be flipped
static void interpret_next_line(std::ifstream* input, bool flip)
{
	int inst_crc = 0;
	int arg = 0;
	get_instruction(&inst_crc, &arg, *input);
	instructions[inst_crc](arg, *input, flip);
}

// Init global state
static void common_setup() 
{
	Accumulator = 0;

	instructions.emplace(INST_ACC_CRC, acc);
	instructions.emplace(INST_JMP_CRC, jmp);
	instructions.emplace(INST_NOP_CRC, nop);

	Current_line = 0;
	Line_flipped = -1;

	std::fill(std::begin(Visited_lines), std::end(Visited_lines), false);
	while (!Frame_stack.empty()) {
		Frame_stack.pop();
	}
}

/*
* A strange infinite loop in the boot code (your puzzle input) of the device.
* You should be able to fix it, but first you need to be able to run the code in isolation.
* The boot code is represented as a text file with one instruction per line of text.
* Each instruction consists of an operation (acc, jmp, or nop) and an argument (a signed number like +4 or -20).
* acc increases or decreases a single global value called the accumulator by the value given in the argument.
*   After an acc instruction, the instruction immediately below it is executed next.
* jmp jumps to a new instruction relative to itself. The next instruction to execute is found using the argument as an offset from the jmp instruction
* nop stands for No OPeration - it does nothing. The instruction immediately below it is executed next.
*/

// Immediately before any instruction is executed a second time, what value is in the accumulator
void problem_1::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	common_setup();

	// Loop through until we run into a line we've visited before
	while (!input.eof()) {
		if (Visited_lines[Current_line]) {
			break;
		}
		Visited_lines[Current_line] = true;
		interpret_next_line(&input, false);
	}
	input.close();

	std::string answer;
	answer = std::to_string(Accumulator);
	output_answer(answer);
}

// Reverse the state to before the most recent line we can flip
//
// input:	The input to reverse
void reverse_to_before_flip_line(std::ifstream* input) 
{
	frame cur_frame = Frame_stack.top();	
	// Undo any previous flip
	if (Line_flipped != -1) {
		// Remove frames until the one where we flipped
		while (cur_frame.m_line != Line_flipped) {
			Frame_stack.pop();
			cur_frame = Frame_stack.top();
		}
		Frame_stack.pop();
		cur_frame = Frame_stack.top();
	}

	// Move input back to position
	input->seekg(cur_frame.m_pos);
	int inst_crc = 0;
	int arg = 0;
	get_instruction(&inst_crc, &arg, *input);	

	// Don't care about accumulator instructions or when flipping does nothing
	while (inst_crc == INST_ACC_CRC || arg == 0) {
		Frame_stack.pop();
		cur_frame = Frame_stack.top();
		input->seekg(cur_frame.m_pos);
		get_instruction(&inst_crc, &arg, *input);
	}

	Accumulator = cur_frame.m_acc;
	Current_line = cur_frame.m_line;
}

/*
* Exactly one instruction is corrupted.
* Either a jmp is supposed to be a nop, or a nop is supposed to be a jmp
*/

// What is the value of the accumulator after the program terminates
void problem_2::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	common_setup();

	while (!input.eof()) {
		bool flip = false;
		// Once we've found a line we've visited
		if (Visited_lines[Current_line]) {
			// Something went wrong
			// Reverse what's happened to before we made the last jmp or nop
			reverse_to_before_flip_line(&input);
			Line_flipped = Current_line;
			// Make the next instruction be flipped
			flip = true;
		}

		Visited_lines[Current_line] = true;
		if (!flip) {
			// If we didn't just revert, then add this to the stack
			Frame_stack.emplace(frame(Accumulator, Current_line, input.tellg()));
		}
		interpret_next_line(&input, flip);
	}

	input.close();

	std::string answer;
	answer = std::to_string(Accumulator);
	output_answer(answer);
}