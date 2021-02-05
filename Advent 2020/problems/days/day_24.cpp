#include "day_24.h"

#include "../common_includes.h"

#include <set>

PROBLEM_CLASS_CPP(24);

typedef std::pair<int, int> coord;
typedef std::set<coord> tiles;

/*
	W and E will always be -1,0 and 1,0
	NE and NW will always be X,1
		W will be 1,1 for y >= 0 and 0,1 otherwise
		E will be -1,1 for y < 0 and 0,1 otherwise
	SE and SW will always be X,-1
		W will be 1,-1 for y <= 0 and 0,1 otherwise
		E will be -1,-1 for y > 0 and 0,1 otherwise
 
	   -2  -1  0   1
	 / * / * / * / *  2
    * / * / * / *    1
  * | * | * | *      0
    * \ * \ * \ *    -1
    \ * \ * \ * \ *  -2
*/

enum class direction {
	W = 0,
	NW,
	NE,
	E,
	SE,
	SW,
	NUM_DIRECTIONS
};
direction string_to_direction(const std::string& dir_s)
{
	if (dir_s.empty()) {
		return direction::NUM_DIRECTIONS;
	}

	switch (dir_s[0])
	{
	case 'w':
		return direction::W;
	case 'e':
		return direction::E;
	case 'n':
		if (dir_s.size() == 2) {
			return dir_s[1] == 'w' ? direction::NW : direction::NE;
		}
		break;
	case 's':
		if (dir_s.size() == 2) {
			return dir_s[1] == 'w' ? direction::SW : direction::SE;
		}
		break;
	default:
		break;
	}
	return direction::NUM_DIRECTIONS;
}

// Determine the coordinates for the neighbor in a given direction
//
// cur_coord:		Coord to start from
// neighbor_dir:	Direction to get
//
// Returns the coord of the neighbor
coord get_neighbor_coord(const coord& cur_coord, direction neighbor_dir)
{
	coord neighbor_coord = cur_coord;
	// See the block note at the top for how this is determined
	switch (neighbor_dir)
	{
	case direction::W:
		--neighbor_coord.first;
		break;
	case direction::E:
		++neighbor_coord.first;
		break;
	case direction::NW:
		if (neighbor_coord.second >= 0) {
			--neighbor_coord.first;
		}
		++neighbor_coord.second;
		break;
	case direction::NE:
		if (neighbor_coord.second < 0) {
			++neighbor_coord.first;
		}
		++neighbor_coord.second;
		break;
	case direction::SW:
		if (neighbor_coord.second <= 0) {
			--neighbor_coord.first;
		}
		--neighbor_coord.second;
		break;
	case direction::SE:
		if (neighbor_coord.second > 0) {
			++neighbor_coord.first;
		}
		--neighbor_coord.second;
		break;
	default:
		break;
	}
	return neighbor_coord;
}

// Get the next direction from the input
//
// cur_index:	The index we're currently at
// directions:	The direction string
//
// Returns the next direction
direction get_next_direction_from_path(size_t* cur_index, const std::string& directions)
{
	if (directions[*cur_index] == 'n' || directions[*cur_index] == 's') {
		*cur_index += 2;
		return string_to_direction(directions.substr((*cur_index) - 2, 2));
	}
	return string_to_direction(directions.substr((*cur_index)++, 1));
}

// Get the coordinate for the direction input
//
// directions:	List of directions to get to the tile
//
// Returns the coordinate of the tile the direction points to
coord get_tile_coord(const std::string& directions)
{
	coord tile_coord;
	size_t cur_dir_index = 0;
	while (cur_dir_index < directions.size()) {
		tile_coord = get_neighbor_coord(tile_coord, get_next_direction_from_path(&cur_dir_index, directions));
	}
	return tile_coord;
}

// Whether the given tile should flip
//
// cur_black_tiles:		The current black tile state
// cur_coord:				Coord to check
// is_currently_white:	Whether the tile is currently white
//
// Returns true if the tile should flip
bool tile_should_flip(const tiles& cur_black_tiles, const coord& cur_coord, bool is_currently_white)
{
	// Count the amount of black tile neighbors
	unsigned int num_black_neighbors = 0;
	for (size_t cur_dir = 0; cur_dir < static_cast<size_t>(direction::NUM_DIRECTIONS); ++cur_dir) {
		if (cur_black_tiles.find(get_neighbor_coord(cur_coord, static_cast<direction>(cur_dir))) != cur_black_tiles.end()) {
			++num_black_neighbors;
		}
		// Black tiles flip if more than 2 neighbors are black
		if (!is_currently_white && num_black_neighbors == 3) {
			return true;
		}
	}
	// White tiles flip if exactly 2 are black, black tiles flip if exactly 0 are black
	return is_currently_white ? num_black_neighbors == 2 : num_black_neighbors == 0;
}

// Get the next day's black tiles
//
// next_day_black_tiles:	(Output) Next day's black tiles
// cur_black_tiles:			The current black tiles
void get_next_day_black_tiles(tiles* next_day_black_tiles, const tiles& cur_black_tiles)
{
	tiles white_tiles;
	for (const coord& cur_tile_coord : cur_black_tiles) {
		// If the current black tile shouldn't flip, add it to the next day's
		if (!tile_should_flip(cur_black_tiles, cur_tile_coord, false)) {
			next_day_black_tiles->insert(cur_tile_coord);
		}

		// Go through neighbors
		unsigned int num_black_neighbors = 0;
		for (size_t cur_dir = 0; cur_dir < static_cast<size_t>(direction::NUM_DIRECTIONS); ++cur_dir) {
			coord neighbor_coord = get_neighbor_coord(cur_tile_coord, static_cast<direction>(cur_dir));
			// Neighbor is black tile, going to check it later
			if (cur_black_tiles.find(neighbor_coord) != cur_black_tiles.end()) {
				continue;
			}
			// Already checked this white tile
			if (white_tiles.find(neighbor_coord) != white_tiles.end()) {
				continue;
			}
			white_tiles.insert(neighbor_coord);
			// If this white tile should flip, add to next day's black tiles
			if (tile_should_flip(cur_black_tiles, neighbor_coord, true)) {
				next_day_black_tiles->insert(neighbor_coord);
			}
		}
	}
}

/*
* The tiles are all hexagonal; they need to be arranged in a hex grid with a very specific color pattern
* The tiles are all white on one side and black on the other.
* They start with the white side facing up. The lobby is large enough to fit whatever pattern might need to appear there
* A member of the renovation crew gives you a list of the tiles that need to be flipped over (your puzzle input).
* Each line in the list identifies a single tile that needs to be flipped by giving a series of steps starting from a reference tile in the very center of the room
* Every tile has six neighbors: east, southeast, southwest, west, northwest, and northeast
* A tile is identified by a series of these directions with no delimiters
* Each time a tile is identified, it flips from white to black or from black to white
*/

// How many tiles are left with the black side up
void problem_1::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	tiles cur_black_tiles;
	while (!input.eof()) {
		std::string next_tile;
		std::getline(input, next_tile);
		coord next_tile_id = get_tile_coord(next_tile);
		tiles::iterator black_tile = cur_black_tiles.find(next_tile_id);
		// First time finding this tile, make it black
		if (black_tile == cur_black_tiles.end()) {
			cur_black_tiles.insert(next_tile_id);
		} else {
			// Already found, flip it again
			cur_black_tiles.erase(black_tile);
		}
	}
	input.close();

	std::string answer;
	answer = std::to_string(cur_black_tiles.size());
	output_answer(answer);
}

/*
* Any black tile with zero or more than 2 black tiles immediately adjacent to it is flipped to white.
* Any white tile with exactly 2 black tiles immediately adjacent to it is flipped to black.
* Here, tiles immediately adjacent means the six tiles directly touching the tile in question.
* The rules are applied simultaneously to every tile
*/

// How many tiles will be black after 100 days
void problem_2::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	// Get the initial state
	tiles cur_black_tiles;
	while (!input.eof()) {
		std::string next_tile;
		std::getline(input, next_tile);
		coord next_tile_id = get_tile_coord(next_tile);
		tiles::iterator black_tile = cur_black_tiles.find(next_tile_id);
		if (black_tile == cur_black_tiles.end()) {
			// First time finding this tile, make it black
			cur_black_tiles.insert(next_tile_id);
		} else {
			// Already found, flip it again
			cur_black_tiles.erase(black_tile);
		}
	}
	input.close();

	// Flip for 100 days
	for (int day = 0; day < 100; ++day)	{
		tiles next_day_black_tiles;
		get_next_day_black_tiles(&next_day_black_tiles, cur_black_tiles);
		cur_black_tiles.swap(next_day_black_tiles);
	}

	std::string answer;
	answer = std::to_string(cur_black_tiles.size());
	output_answer(answer);
}