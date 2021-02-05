#include "day_3.h"

#include "../common_includes.h"

#include <vector>

PROBLEM_CLASS_CPP(3);

// Count trees for a given slope
//
// map:			The map of trees to traverse
// x_velocity:	The x velocity of the slope
// y_velocity:	The y velocity of the slope
//
// Returns the number of tress along the slope
unsigned int traverse_slope(std::vector<std::string>& map, unsigned int x_velocity, unsigned int y_velocity) {
	static const char TREE_CHAR = '#';

	unsigned int trees = 0;
	unsigned int width = map[0].length();

	// Go until we reach the bottom of the map
	unsigned int x = 0;
	for (unsigned int y = y_velocity; y < map.size(); y += y_velocity) {
		x += x_velocity;
		// Wrap around width
		if (x >= width) {
			x = x % width;
		}

		if (map[y][x] == TREE_CHAR) {
			++trees;
		}
	}

	return trees;
}

/*
* You make a map (your puzzle input) of the open squares (.) and trees (#) you can see.
* The same pattern repeats to the right infinite times
* You start on the open square (.) in the top-left corner and need to reach the bottom (below the bottom-most row on your map).
*/

//	Count all the trees you would encounter for the slope right 3, down 1
void problem_1::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}
	
	std::vector<std::string> map;

	// Get each line of th emap
	while (!input.eof()) {
		std::string map_line;
		std::getline(input, map_line);

		map.push_back(map_line);
	}

	input.close();

	if (map.empty()) {
		return;
	}

	unsigned int trees = traverse_slope(map, 3, 1);
	output_answer(std::to_string(trees));
}

/*
* Determine the number of trees you would encounter if, for each of the following slopes, you start at the top-left corner and traverse the map all the way to the bottom:
* Right 1, down 1.
* Right 3, down 1. (This is the slope you already checked.)
* Right 5, down 1.
* Right 7, down 1.
* Right 1, down 2.
*/

// What do you get if you multiply together the number of trees encountered on each of the listed slopes?
void problem_2::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	std::vector<std::string> map;

	// Get each line of th emap
	while (!input.eof()) {
		std::string map_line;
		std::getline(input, map_line);

		map.push_back(map_line);
	}
	input.close();

	if (map.empty()) {
		return;
	}

	unsigned int trees_mult = traverse_slope(map, 1, 1) * traverse_slope(map, 3, 1) * traverse_slope(map, 5, 1) * traverse_slope(map, 7, 1) * traverse_slope(map, 1, 2);
	output_answer(std::to_string(trees_mult));
}