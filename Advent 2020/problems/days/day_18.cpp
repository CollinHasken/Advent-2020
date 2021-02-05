#include "day_18.h"

#include "../common_includes.h"

#include <stack>

PROBLEM_CLASS_CPP(18);

// Different operations
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

// Perform any remaining multiplication operations left on the stack
//
// sum:			(Output) The resulting sum after any more multiplications
// op_stack:	The stack of operations remaining
static void resolve_mults_for_context(long long int* sum, std::stack<stack_info>* op_stack)
{
	// Nothing left to do
	if (op_stack->empty()) {
		return;
	}

	// Do remaining mults
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

// Do the advanced operation
//
// sum:					(Output) The resulting sum
// prev_operation:	(Output) The previous operation that was performed. Will be set to the current one at the end
// op_stack:			(Output) The current operation stack
// input:				The input to read from
static void do_advanced_operation(long long int* sum, operations* prev_operation, std::stack<stack_info>* op_stack, const std::string& input)
{
	// Get the operation
	operations op = string_to_operation(input);

	switch (op)
	{
	case operations::MULT:
	case operations::PLUS:
		// Don't do anything with these, just store that it was the previous operation for next time
		break;
	case operations::PAREN_OPEN:
		// Need to push this state on the stack to isolate the operations in the parenthesis
		// If the previous thing was another open paren, then we don't need to isolate again
		if (*prev_operation != operations::NONE && *prev_operation != operations::PAREN_OPEN) {
			op_stack->push(std::make_pair(*prev_operation, *sum));
		}
		// Push on that we're going within a parenthesis
		op_stack->push(std::make_pair(operations::NONE, 0));
		*sum = 0;
		break;
	case operations::PAREN_CLOSE:
	{
		// Now that we're closing a context, we need to resolve any remaining multiplications
		resolve_mults_for_context(sum, op_stack);

		// Remove the dummy stack entry to signify parenthesis
		op_stack->pop();
		if (op_stack->empty()) {
			break;
		}
		stack_info prev_stack = op_stack->top();

		switch (prev_stack.first)
		{
		case operations::PLUS:
			// If this paren was going to be added, go ahead and add it
			*sum += prev_stack.second;
			op_stack->pop();
			break;
		default:
			// If it was multiplication, we need to delay
			break;
		}
	}
	break;
	case operations::NUM:
	{
		// Now that we have a number
		int num = std::stoi(input);
		switch (*prev_operation)
		{
		case operations::PLUS:
			// Simply add
			*sum += num;
			break;
		case operations::MULT:
			// If we're trying to multiply by this number, we need to put it on the stack to delay till later
			op_stack->push(std::make_pair(*prev_operation, *sum));
			*prev_operation = operations::NONE;
		case operations::PAREN_OPEN:
		case operations::NONE:
			// If it was an open parenthesis or nothing, then we're initializing
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
	// If there's another operation within this input, do it
	if (op != operations::NUM && input.size() > 1) {
		do_advanced_operation(sum, prev_operation, op_stack, input.substr(1));
	} else if (op == operations::NUM && string_to_operation(input.back()) == operations::PAREN_CLOSE) {
		// If there's a ')' on the end of this number, we need to do the close paren operation
		int non_num_index = 0;
		while (string_to_operation(input[++non_num_index]) == operations::NUM) {}
		do_advanced_operation(sum, prev_operation, op_stack, input.substr(non_num_index));
	}
}

// Do the operation
//
// sum:					(Output) The resulting sum
// prev_operation:	(Output) The previous operation that was performed. Will be set to the current one at the end
// op_stack:			(Output) The current operation stack
// input:				The input to read from
static void do_operation(long long int* sum, operations* prev_operation, std::stack<stack_info>* op_stack, const std::string& input)
{
	// Get the operation
	operations op = string_to_operation(input);

	switch (op)
	{
	case operations::PLUS:
	case operations::MULT:
		// Don't do anything with these, just store that it was the previous operation for next time
		break;
	case operations::PAREN_OPEN:
		// Need to push this state on the stack to isolate the operations in the parenthesis
		op_stack->push(std::make_pair(*prev_operation, *sum));
		*sum = 0;
		break;
	case operations::PAREN_CLOSE:
	{
		// Now that this is done, get the previous stack info and do the operation that was planned for this resultant
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
		// Now that we have a number
		int num = std::stoi(input);
		switch (*prev_operation)
		{
		case operations::PLUS:
			// Add or multiply if that was the previous operation
			*sum += num;
			break;
		case operations::MULT:
			*sum *= num;
			break;
		case operations::PAREN_OPEN:
		case operations::NONE:
		// If it was an open parenthesis or nothing, then we're initializing
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

	// If there's another operation within this input, do it
	if (op != operations::NUM && input.size() > 1) {
		do_operation(sum, prev_operation, op_stack, input.substr(1));
	} else if (op == operations::NUM && string_to_operation(input.back()) == operations::PAREN_CLOSE) {
		// If there's a ')' on the end of this number, we need to do the close paren operation
		int non_num_index = 0;
		while(string_to_operation(input[++non_num_index]) == operations::NUM){}
		do_operation(sum, prev_operation, op_stack, input.substr(non_num_index));
	}
}

/*
* The homework (your puzzle input) consists of a series of expressions that consist of addition (+), multiplication (*), and parentheses ((...))
* The rules of operator precedence have changed. 
* Rather than evaluating multiplication before addition, the operators have the same precedence, and are evaluated left-to-right regardless of the order in which they appear
*/

// What is the sum of the resulting values
void problem_1::solve(const std::string& file_name)
{
	std::ifstream input_file(file_name);

	long long int sum = 0;
	// Do the equations
	while (!input_file.eof()) {
		operations prev_operation = operations::NONE;
		std::stack<stack_info> op_stack;
		std::string input_line;
		std::getline(input_file, input_line);

		long long int cur_sum = 0;
		std::istringstream input_line_stream(input_line);
		// Do each operations
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

/*
* Now, addition and multiplication have different precedence levels, but they're not the ones you're familiar with.
* Instead, addition is evaluated before multiplication
*/

// What do you get if you add up the results of evaluating the homework problems using these new rules
void problem_2::solve(const std::string& file_name)
{
	std::ifstream input_file(file_name);

	long long int sum = 0;
	// Do the equations
	while (!input_file.eof()) {
		operations prev_operation = operations::NONE;
		std::stack<stack_info> op_stack;
		std::string input_line;
		std::getline(input_file, input_line);

		long long int cur_sum = 0;
		std::istringstream input_line_stream(input_line);
		// Do each operations
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