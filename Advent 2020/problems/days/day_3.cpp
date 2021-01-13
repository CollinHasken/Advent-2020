#include "day_3.h"

#include "../common_includes.h"

#include <vector>

unsigned int traverse_slop(std::vector<std::string>& map, unsigned int x_velocity, unsigned int y_velocity) {
	static const char TREE_CHAR = '#';

	unsigned int trees = 0;
	unsigned int width = map[0].length();

	unsigned int x = 0;
	for (unsigned int y = y_velocity; y < map.size(); y += y_velocity) {
		x = (x + x_velocity) % width;
		if (map[y][x] == TREE_CHAR) {
			++trees;
		}
	}

	return trees;
}

PROBLEM_CLASS_CPP(3);
void problem_1::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}
	
	std::vector<std::string> map;

	while (!input.eof()) {
		std::string map_line;
		std::getline(input, map_line);

		map.push_back(map_line);
	}

	input.close();

	if (map.empty()) {
		return;
	}

	unsigned int trees = traverse_slop(map, 3, 1);

	std::cout << trees;
}

void problem_2::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	std::vector<std::string> map;

	while (!input.eof()) {
		std::string map_line;
		std::getline(input, map_line);

		map.push_back(map_line);
	}

	input.close();

	if (map.empty()) {
		return;
	}

	unsigned int trees_mult = traverse_slop(map, 1, 1) * traverse_slop(map, 3, 1) * traverse_slop(map, 5, 1) * traverse_slop(map, 7, 1) * traverse_slop(map, 1, 2);

	std::cout << trees_mult;
}