#include "day_16.h"

#include "../common_includes.h"

#include <algorithm>
#include <map>
#include <set>
#include <vector>

PROBLEM_CLASS_CPP(16);

typedef int field;
typedef std::pair<int, int> range;
typedef std::vector<range> full_range;
typedef std::map<std::string, field> field_ind;
typedef std::map<field, full_range> field_range;
bool number_is_valid(int val, const field_range& field_ranges);

// Ticket to hold the different fields
class ticket
{
public:
	bool init(const field_range& field_ranges, const std::string& input);
	std::vector<int> field_values;
};

// Initialize a ticket with valid ranges and input
//
// field_ranges:	Valid field ranges
// input:			Input to read from
//
// Returns true if a valid ticket was created
bool ticket::init(const field_range& field_ranges, const std::string& input)
{
	std::istringstream input_line_stream(input);
	int num;
	while (!input_line_stream.eof()) {
		input_line_stream >> num;
		input_line_stream.ignore();
		if (!number_is_valid(num, field_ranges)) {
			return false;
		}
		field_values.push_back(num);
	}
	return true;
}

// Determine if a value is valid within any of the ranges
//
// val:				Number to check
// field_ranges:	Valid ranges to check against
//
// Returns true if its valid
bool number_is_valid(int val, const field_range& field_ranges)
{
	for (const auto& field_range_iter : field_ranges) {
		for (const auto& range_iter : field_range_iter.second) {
			if (val >= range_iter.first && val <= range_iter.second) {
				return true;
			}
		}
	}
	return false;
}

// Get the valid fields that the given value could be within
//
// valid_fields:	(Output) The fields that are valid for the value
// val:				The value to check with
// field_ranges:	The ranges to check against
void get_valid_fields(std::set<field>* valid_fields, int val, const field_range& field_ranges)
{
	for (const auto& field_range_iter : field_ranges) {
		for (const auto& range_iter : field_range_iter.second) {
			if (val >= range_iter.first && val <= range_iter.second) {
				valid_fields->emplace(field_range_iter.first);
				break;
			}
		}
	}
}

// Remove any fields that are already known from other valid fields' possibilities
//
// valid_fields:	Everyone's valid fields 
// field_index:	
void remove_used_field(std::vector<std::set<field>>& valid_fields, size_t field_index)
{
	const field& cur_field = *(valid_fields[field_index].begin());
	for (size_t i = 0; i < valid_fields.size(); ++i) {
		if (i == field_index) {
			continue;
		}
		auto found_iter = valid_fields[i].find(cur_field);
		if (found_iter != valid_fields[i].end()) {
			valid_fields[i].erase(found_iter);
			// If this is now known which field and we already passed this index, call the function again
			if (i < field_index && valid_fields[i].size() == 1) {
				remove_used_field(valid_fields, i);
			}
		}
	}
}

/*
* Collect the rules for ticket fields, the numbers on your ticket, 
* and the numbers on other nearby tickets for the same train service (via the airport security cameras)
* The rules for ticket fields specify a list of fields that exist somewhere on the ticket and the valid ranges of values for each field
* '?' represents text in a language you don't understand
* The first number is always the same specific field, the second number is always a different specific field
* Start by determining which tickets are completely invalid; these are tickets that contain values which aren't valid for any field. Ignore your ticket for now
*/

// Consider the validity of the nearby tickets you scanned. What is your ticket scanning error rate
void problem_1::solve(const std::string& file_name)
{
	int ticket_error = 0;
	field_range field_ranges;
	std::ifstream input_file(file_name);

	std::string input_line;
	std::getline(input_file, input_line);

	// Extract the fields and their ranges
	field field_num = 0;
	while (!input_line.empty()) {
		std::istringstream input_line_stream(input_line);

		std::string field_s;
		std::getline(input_line_stream, field_s, ':');

		full_range cur_range;
		int min, max;
		while (!input_line_stream.eof()) {
			input_line_stream >> min;
			input_line_stream.ignore();
			input_line_stream >> max;
			input_line_stream.ignore(4);

			cur_range.emplace_back(std::make_pair(min, max));
		}
		field_ranges.emplace(field_num++, cur_range);
		std::getline(input_file, input_line);
	}

	input_file.ignore(600, '\n');

	// My ticket
	input_file.ignore(600, '\n');

	input_file.ignore(600, '\n');
	input_file.ignore(600, '\n');

	// Nearby tickets
	while (!input_file.eof()) {
		std::getline(input_file, input_line);
		std::istringstream input_line_stream(input_line);
		int num;
		while (!input_line_stream.eof()) {
			input_line_stream >> num;
			input_line_stream.ignore();
			if (!number_is_valid(num, field_ranges)) {
				ticket_error += num;
			}
		}
	}
	input_file.close();

	std::string answer;
	answer = std::to_string(ticket_error);
	output_answer(answer);
}

/*
* Now that you've identified which tickets contain invalid values, discard those tickets entirely.
* Use the remaining valid tickets to determine which field is which
* Using the valid ranges for each field, determine what order the fields appear on the tickets.
* The order is consistent between all tickets: if seat is the third field, it is the third field on every ticket, including your ticket.
*/

// What do you get if you multiply the six fields that start with the word 'departure' values together
void problem_2::solve(const std::string& file_name)
{
	field_range field_ranges;
	field_ind field_indecies;
	std::ifstream input_file(file_name);

	std::string input_line;
	std::getline(input_file, input_line);

	// Extract the fields and their ranges
	field field_num = 0;
	while (!input_line.empty()) {
		std::istringstream input_line_stream(input_line);

		std::string field_s;
		std::getline(input_line_stream, field_s, ':');
		field_indecies.emplace(field_s, field_num);

		full_range cur_range;
		int min, max;
		while (!input_line_stream.eof()) {
			input_line_stream >> min;
			input_line_stream.ignore();
			input_line_stream >> max;
			input_line_stream.ignore(4);

			cur_range.emplace_back(std::make_pair(min, max));
		}
		field_ranges.emplace(field_num++, cur_range);
		std::getline(input_file, input_line);
	}

	input_file.ignore(600, '\n');

	// My ticket
	std::getline(input_file, input_line);
	ticket my_ticket;
	my_ticket.init(field_ranges, input_line);

	input_file.ignore(600, '\n');
	input_file.ignore(600, '\n');

	// Nearby tickets
	std::vector<ticket> nearby_tickets;
	while (!input_file.eof()) {
		std::getline(input_file, input_line);
		ticket cur_ticket;
		if (cur_ticket.init(field_ranges, input_line)) {
			nearby_tickets.push_back(cur_ticket);
		}
	}
	input_file.close();

	// Figure out order
	std::vector<std::set<field>> possible_fields;
	for (const auto& nearby_ticket : nearby_tickets) {
		// Go through each value 
		for (size_t i = 0; i < nearby_ticket.field_values.size(); ++i) {
			// Get the possible fields for this value
			std::set<field> valid_fields;
			get_valid_fields(&valid_fields, nearby_ticket.field_values[i], field_ranges);
			// If there aren't any possibilities yet, just add these
			if (i >= possible_fields.size()) {
				possible_fields.push_back(valid_fields);
			} else {
				// Intersect the valid fields for this value and the current possible fields
				std::set<field> intersection;
				auto it = std::set_intersection(valid_fields.cbegin(), valid_fields.cend(), possible_fields[i].cbegin(), possible_fields[i].cend(), std::inserter(intersection, intersection.begin()));
				possible_fields[i].swap(intersection);
			}
		}

		// Remove known fields from possible fields
		for (size_t i = 0; i < possible_fields.size(); ++i) {
			if (possible_fields[i].size() == 1) {
				remove_used_field(possible_fields, i);
			}
		}

		// If there's only one possible field per value, then the fields are figured out
		bool figured_out = true;
		for (const auto& cur_possible_fields : possible_fields) {
			if (cur_possible_fields.size() != 1) {
				figured_out = false;
				break;
			}
		}
		if (figured_out) {
			break;
		}
	}

	long long int ans = 1;
	for (const auto& field_index_pair : field_indecies) {
		// For each departure field
		if (field_index_pair.first.find("departure") != std::string::npos) {
			// Find the position for that field num
			for (size_t real_index = 0; real_index < possible_fields.size(); ++real_index) {
				if (*possible_fields[real_index].begin() == field_index_pair.second) {
					ans *= my_ticket.field_values[real_index];
					break;
				}
			}
		}
	}

	std::string answer;
	answer = std::to_string(ans);
	output_answer(answer);
}