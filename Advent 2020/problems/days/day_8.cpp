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

static int code_to_crc(const char code[4])
{
	int crc = 0;
	for (int i = 0; i < 4; ++i) crc += static_cast<int>(code[i]);
	return crc;
}

static int code_to_crc(const std::string& code)
{
	int crc = 0;
	for (const char c : code) crc += static_cast<int>(c);
	return crc;
}

static const int INST_ACC_CRC = code_to_crc(INST_ACC);
static const int INST_JMP_CRC = code_to_crc(INST_JMP);
static const int INST_NOP_CRC = code_to_crc(INST_NOP);

struct frame {
	frame(int acc, int line, std::streampos pos) : m_acc(acc), m_line(line), m_pos(pos) { };
	int m_acc;
	int m_line;
	std::streampos m_pos;
};
static std::stack<frame> Frame_stack;

static void inline next_line(std::ifstream& input)
{
	input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	++Current_line;
}

static void inline prev_line(std::ifstream& input)
{
	input.seekg(-3,std::ios_base::cur); //\n and last char
	while (input.peek() != '\n') {
		input.seekg(-1, std::ios_base::cur);
	}
	input.ignore();
	--Current_line;
}

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

static void nop(int arg, std::ifstream& input, bool flip)
{
	if (flip) {
		jmp(arg, input, false);
	} else {
		next_line(input);
	}
}

static void get_instruction(int* inst_crc, int* arg, std::ifstream& input)
{
	std::streampos pos = input.tellg();
	std::string instruction_line;
	std::getline(input, instruction_line);
	input.seekg(pos);

	std::string inst = instruction_line.substr(0, 3);
	bool positive = instruction_line[4] == PLUS;

	*arg = std::stoi(instruction_line.substr(5)) * (positive ? 1 : -1);
	*inst_crc = code_to_crc(inst);
}

static void interpret_next_line(std::ifstream* input, bool flip)
{
	int inst_crc = 0;
	int arg = 0;
	get_instruction(&inst_crc, &arg, *input);
	instructions[inst_crc](arg, *input, flip);
}

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

void problem_1::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	common_setup();

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

void reverse_to_before_flip_line(std::ifstream* input) 
{
	frame cur_frame = Frame_stack.top();	
	if (Line_flipped != -1) {
		while (cur_frame.m_line != Line_flipped) {
			Frame_stack.pop();
			cur_frame = Frame_stack.top();
		}
		Frame_stack.pop();
		cur_frame = Frame_stack.top();
	}

	input->seekg(cur_frame.m_pos);
	int inst_crc = 0;
	int arg = 0;
	get_instruction(&inst_crc, &arg, *input);	

	while (inst_crc == INST_ACC_CRC || arg == 0) {
		Frame_stack.pop();
		cur_frame = Frame_stack.top();
		input->seekg(cur_frame.m_pos);
		get_instruction(&inst_crc, &arg, *input);
	}

	Accumulator = cur_frame.m_acc;
	Current_line = cur_frame.m_line;
}

void problem_2::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	common_setup();

	while (!input.eof()) {
		bool flip = false;
		if (Visited_lines[Current_line]) {
			reverse_to_before_flip_line(&input);
			Line_flipped = Current_line;
			flip = true;
		}

		Visited_lines[Current_line] = true;
		if (!flip) {
			Frame_stack.emplace(frame(Accumulator, Current_line, input.tellg()));
		}
		interpret_next_line(&input, flip);
	}

	input.close();

	std::string answer;
	answer = std::to_string(Accumulator);
	output_answer(answer);
}