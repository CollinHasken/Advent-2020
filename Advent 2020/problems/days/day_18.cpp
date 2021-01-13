#include "day_18.h"

#include "../common_includes.h"

#include <stack>

PROBLEM_CLASS_CPP(18);


enum class operations {
	PLUS,
	MULT,
	PAREN_OPEN,
	PAREN_CLOSE,
	NUM,
	NONE,
};
typedef std::pair<operations, long long int> stack_info;

static operations string_to_operation(char operation_char)
{
	switch (operation_char)
	{
	case '+':
		return operations::PLUS;
	case '*':
		return operations::MULT;
	case '(':
		return operations::PAREN_OPEN;
	case ')':
		return operations::PAREN_CLOSE;
	default:
		return operations::NUM;
	}
}

static operations string_to_operation(const std::string& operation_string)
{
	return string_to_operation(operation_string[0]);
}

static void resolve_mults_for_context(long long int* sum, std::stack<stack_info>* op_stack)
{
	if (op_stack->empty()) {
		return;
	}
	stack_info prev_stack = op_stack->top();
	while (prev_stack.first == operations::MULT) {
		op_stack->pop();
		*sum *= prev_stack.second;
		if (op_stack->empty()) {
			return;
		}
		prev_stack = op_stack->top();
	}
}

static void do_advanced_operation(long long int* sum, operations* prev_operation, std::stack<stack_info>* op_stack, const std::string& input)
{
	operations op = string_to_operation(input);

	switch (op)
	{
	case operations::MULT:
	case operations::PLUS:
		break;
	case operations::PAREN_OPEN:
		if (*prev_operation != operations::NONE && *prev_operation != operations::PAREN_OPEN) {
			op_stack->push(std::make_pair(*prev_operation, *sum));
		}
		op_stack->push(std::make_pair(operations::NONE, 0));
		*sum = 0;
		break;
	case operations::PAREN_CLOSE:
	{
		resolve_mults_for_context(sum, op_stack);

		op_stack->pop();
		if (op_stack->empty()) {
			break;
		}
		stack_info prev_stack = op_stack->top();

		switch (prev_stack.first)
		{
		case operations::PLUS:
			*sum += prev_stack.second;
			op_stack->pop();
			break;
		default:
			break;
		}
	}
	break;
	case operations::NUM:
	{
		int num = std::stoi(input);
		switch (*prev_operation)
		{
		case operations::PLUS:
			*sum += num;
			break;
		case operations::MULT:
			op_stack->push(std::make_pair(*prev_operation, *sum));
			*prev_operation = operations::NONE;
		case operations::PAREN_OPEN:
		case operations::NONE:
			*sum = num;
			break;
		default:
			break;
		}
	}
	break;
	default:
		break;
	}

	*prev_operation = op;
	if (op != operations::NUM && input.size() > 1) {
		do_advanced_operation(sum, prev_operation, op_stack, input.substr(1));
	} else if (op == operations::NUM && string_to_operation(input.back()) == operations::PAREN_CLOSE) {
		int non_num_index = 0;
		while (string_to_operation(input[++non_num_index]) == operations::NUM) {}
		do_advanced_operation(sum, prev_operation, op_stack, input.substr(non_num_index));
	}
}

static void do_operation(long long int* sum, operations* prev_operation, std::stack<stack_info>* op_stack, const std::string& input)
{
	operations op = string_to_operation(input);

	switch (op)
	{
	case operations::PLUS:
	case operations::MULT:
		break;
	case operations::PAREN_OPEN:
		op_stack->push(std::make_pair(*prev_operation, *sum));
		*sum = 0;
		break;
	case operations::PAREN_CLOSE:
	{
		stack_info prev_stack = op_stack->top();
		op_stack->pop();

		switch (prev_stack.first)
		{
		case operations::PLUS:
			*sum += prev_stack.second;
			break;
		case operations::MULT:
			*sum *= prev_stack.second;
			break;
		default:
			break;
		}
	}
		break;
	case operations::NUM:
	{
		int num = std::stoi(input);
		switch (*prev_operation)
		{
		case operations::PLUS:
			*sum += num;
			break;
		case operations::MULT:
			*sum *= num;
			break;
		case operations::PAREN_OPEN:
		case operations::NONE:
			*sum = num;
			break;
		default:
			break;
		}
	}
		break;
	default:
		break;
	}

	*prev_operation = op;
	if (op != operations::NUM && input.size() > 1) {
		do_operation(sum, prev_operation, op_stack, input.substr(1));
	} else if (op == operations::NUM && string_to_operation(input.back()) == operations::PAREN_CLOSE) {
		int non_num_index = 0;
		while(string_to_operation(input[++non_num_index]) == operations::NUM){}
		do_operation(sum, prev_operation, op_stack, input.substr(non_num_index));
	}
}

void problem_1::solve(const std::string& file_name)
{
	std::ifstream input_file(file_name);

	long long int sum = 0;
	while (!input_file.eof()) {
		operations prev_operation = operations::NONE;
		std::stack<stack_info> op_stack;
		std::string input_line;
		std::getline(input_file, input_line);

		long long int cur_sum = 0;
		std::istringstream input_line_stream(input_line);
		while (!input_line_stream.eof()) {
			std::string next_in;
			std::getline(input_line_stream, next_in, ' ');

			if (next_in.empty()) {
				continue;
			}

			do_operation(&cur_sum, &prev_operation, &op_stack, next_in);
		}
		sum += cur_sum;
	}

	input_file.close();

	std::string answer;
	answer = std::to_string(sum);
	output_answer(answer);
}

void problem_2::solve(const std::string& file_name)
{
	std::ifstream input_file(file_name);

	long long int sum = 0;
	while (!input_file.eof()) {
		operations prev_operation = operations::NONE;
		std::stack<stack_info> op_stack;
		std::string input_line;
		std::getline(input_file, input_line);

		long long int cur_sum = 0;
		std::istringstream input_line_stream(input_line);
		while (!input_line_stream.eof()) {
			std::string next_in;
			std::getline(input_line_stream, next_in, ' ');

			if (next_in.empty()) {
				continue;
			}

			do_advanced_operation(&cur_sum, &prev_operation, &op_stack, next_in);
		}

		// Do remaining mults
		resolve_mults_for_context(&cur_sum, &op_stack);
		sum += cur_sum;
	}

	input_file.close();

	std::string answer;
	answer = std::to_string(sum);
	output_answer(answer);
}