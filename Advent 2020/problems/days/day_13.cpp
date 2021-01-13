#include "day_13.h"

#include "../common_includes.h"

#include <vector>
#include <set>
#include <tuple>
#include <algorithm>
#include <numeric>

PROBLEM_CLASS_CPP(13);

typedef std::pair<int, int> bus_id_and_time;
typedef long long int lli;

std::pair<lli, lli> dumb_chinese_remainder_theorem(std::pair<lli, lli> an, std::pair<lli, lli> bn)
{
	lli n = 0;
	while ((an.first + (n * an.second)) % bn.second != bn.first) {
		++n;
	}

	lli s = an.first + (n * an.second);
	return std::make_pair(s, an.second * bn.second);
};

void problem_1::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	std::vector<bus_id_and_time> buses;

	std::string input_line;
	std::getline(input, input_line);
	int desired_time = std::stoi(input_line);

	std::getline(input, input_line);
	input.close();

	std::istringstream input_line_stream(input_line);
	std::string bus;
	while (!input_line_stream.eof()) {
		std::getline(input_line_stream, bus, ',');
		if (bus[0] == 'x') {
			continue;
		}
		int bus_id = std::stoi(bus);
		buses.emplace_back(std::make_pair(bus_id, bus_id - (desired_time % bus_id)));
	}

	std::sort(buses.begin(), buses.end(), [](const bus_id_and_time& lhs, const bus_id_and_time& rhs)
		{
			return lhs.second < rhs.second;
		});

	std::string answer;
	answer = std::to_string(buses.front().first * buses.front().second);
	output_answer(answer);
}

void problem_2::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	std::vector<std::pair<lli, lli>> buses;

	std::string input_line;
	std::getline(input, input_line); // Unused
	std::getline(input, input_line);
	input.close();

	std::istringstream input_line_stream(input_line);
	std::string bus;
	int t = -1;
	while (!input_line_stream.eof()) {
		++t;
		std::getline(input_line_stream, bus, ',');
		if (bus[0] == 'x') {
			continue;
		}
		int bus_id = std::stoi(bus);
		buses.emplace_back(std::make_pair(t % bus_id, bus_id));
	}

	std::pair<lli, lli> xn = std::accumulate(std::next(buses.begin(), 1), buses.end(), buses[0], dumb_chinese_remainder_theorem);
	lli n = xn.first;
	lli mod = xn.second;

	// retrieve the smallest possible integer for the final congruence
	lli smallest = mod - n % mod;

	std::string answer;
	answer = std::to_string(smallest);
	output_answer(answer);
}