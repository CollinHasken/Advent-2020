#include "day_17.h"

#include "../common_includes.h"

#include <algorithm>
#include <map>
#include <limits>
#include <vector>

#undef min
#undef max

PROBLEM_CLASS_CPP(17);

typedef std::tuple<int, int, int, int> coord;
coord operator-(const coord& lhs, const coord& rhs)
{
	return std::make_tuple(std::get<0>(lhs) - std::get<0>(rhs), std::get<1>(lhs) - std::get<1>(rhs), std::get<2>(lhs) - std::get<2>(rhs), std::get<3>(lhs) - std::get<3>(rhs));
}

template <int component>
void set_component(coord* cur_coord, const int& rhs, const int& (*compare) (const int&, const int&))
{
	std::get<component>(*cur_coord) = compare(std::get<component>(*cur_coord), rhs);
};

template <int component>
void set_component(coord* cur_coord, const coord& rhs, const int& (*compare) (const int&, const int&))
{
	set_component<component>(cur_coord, std::get<component>(rhs), compare);
};

// Dimensional cube holding state of active and inactive coords
class p_dimension {
public:
	p_dimension(std::ifstream& input, bool four_d = false);

	void boot_up();
	bool is_boot_done() const { return m_cycle == p_dimension::CYCLES_TO_BOOT; }
	
	int get_active_cubes_count() const { return m_active_cubes; }
	void print();

private:
	static constexpr unsigned int CYCLES_TO_BOOT = 6;

	enum class cube_state {
		ACTIVE,
		INACTIVE,
	};

	typedef std::vector<cube_state> dimension_row;
	typedef std::vector<dimension_row> dimension_layer;
	typedef std::vector<dimension_layer> dimension_space;
	typedef std::vector<dimension_space> dimension_hyperspace;

	void next_cycle();
	bool is_coord_valid(const coord& cur_coord) const;
	cube_state get_cube_state(const coord& cur_coord) const;
	cube_state get_next_cube_state(const coord& cur_coord);

	bool m_is_four_d;
	dimension_hyperspace m_hyperspace;
	int m_active_cubes = 0;
	unsigned int m_cycle = 0;

	// The current bbox of our active coords
	coord m_min = std::make_tuple(0, 0, 0, 0);
	coord m_max = std::make_tuple(0, 0, 0, 0);

	// The bbox of the entire lifetime of the cube
	coord m_total_min = std::make_tuple(0, 0, 0, 0);
	coord m_total_max = std::make_tuple(0, 0, 0, 0);
};

// Initialize a dimensional cube with input
//
// input:	Input to initialize with
// four_d:	Whether this cube is 4d or 3d
p_dimension::p_dimension(std::ifstream& input, bool four_d/* = false*/) : m_is_four_d(four_d)
{
	dimension_layer new_layer;
	while (!input.eof()) {
		std::string input_line;
		std::getline(input, input_line);

		dimension_row new_row;
		for (const char cur_cube : input_line) {
			switch (cur_cube)
			{
			case '.':
				new_row.emplace_back(cube_state::INACTIVE);
				break;
			case '#':
				new_row.emplace_back(cube_state::ACTIVE);
				++m_active_cubes;
				break;
			default:
				break;
			}
		}
		new_layer.push_back(new_row);
	}
	// Store our current max bbox
	// The min is 0, 0
	std::get<0>(m_max) = new_layer.empty() ? -1 : new_layer[0].size() - 1;
	std::get<1>(m_max) = new_layer.size() - 1;
	m_total_max = m_max;

	dimension_space new_space;
	new_space.emplace_back(new_layer);
	m_hyperspace.emplace_back(new_space);
}

// Is the coordinate valid without our current bbox
//
// cur_coord:	Coord to test
//
// Returns true if it is valid
bool p_dimension::is_coord_valid(const coord& cur_coord) const
{
	if (std::get<0>(cur_coord) < std::get<0>(m_min) || std::get<0>(cur_coord) > std::get<0>(m_max)) {
		return false;
	}
	if (std::get<1>(cur_coord) < std::get<1>(m_min) || std::get<1>(cur_coord) > std::get<1>(m_max)) {
		return false;
	}
	if (std::get<2>(cur_coord) < std::get<2>(m_min) || std::get<2>(cur_coord) > std::get<2>(m_max)) {
		return false;
	}
	if (std::get<3>(cur_coord) < std::get<3>(m_min) || std::get<3>(cur_coord) > std::get<3>(m_max)) {
		return false;
	}

	return true;
}

// Get the cube state for the given coordinate
//
// cur_coord:	Coord to check
//
// Returns the cube state at that coordinate
p_dimension::cube_state p_dimension::get_cube_state(const coord& cur_coord) const
{
	if (!is_coord_valid(cur_coord)) {
		return cube_state::INACTIVE;
	}
	coord zeroed_coord = cur_coord - m_min;
	return m_hyperspace[std::get<3>(zeroed_coord)][std::get<2>(zeroed_coord)][std::get<1>(zeroed_coord)][std::get<0>(zeroed_coord)];
}

// Get the next cube state for a given coordinate
//
// cur_coord:	Coord to check
//
// Returns the next cube state at that coordinate
p_dimension::cube_state p_dimension::get_next_cube_state(const coord& cur_coord)
{
	int cubes_active = 0;
	// Search all neighbors
	for (int space_offset = (m_is_four_d ? -1 : 0); space_offset < (m_is_four_d ? 2 : 1); ++space_offset) {
		for (int layer_offset = -1; layer_offset < 2; ++layer_offset) {
			for (int row_offset = -1; row_offset < 2; ++row_offset) {
				for (int col_offset = -1; col_offset < 2; ++col_offset) {
					// Don't check ourself
					if ((space_offset | layer_offset | row_offset | col_offset) == 0) {
						continue;
					}
					if (get_cube_state(std::make_tuple(std::get<0>(cur_coord) + col_offset, std::get<1>(cur_coord) + row_offset, std::get<2>(cur_coord) + layer_offset, std::get<3>(cur_coord) + space_offset)) == cube_state::ACTIVE) {
						if (++cubes_active > 3) {
							break;
						}
					}
				}
				if (cubes_active > 3) {
					break;
				}
			}
			if (cubes_active > 3) {
				break;
			}
		}
		if (cubes_active > 3) {
			break;
		}
	}

	return cubes_active == 3 ? cube_state::ACTIVE : (cubes_active == 2 && get_cube_state(cur_coord) == cube_state::ACTIVE ? cube_state::ACTIVE : cube_state::INACTIVE);
}

// Determine the next cycle for the cube
void p_dimension::next_cycle()
{
	m_active_cubes = 0;
	
	dimension_hyperspace next_hyperspace;
	coord next_min = std::make_tuple(std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
	coord next_max = std::make_tuple(std::numeric_limits<int>::min(), std::numeric_limits<int>::min(), std::numeric_limits<int>::min(), std::numeric_limits<int>::min());

	// Get the next state for each coordiante
	for (int space = (m_is_four_d ? std::get<3>(m_min) - 1 : 0); space <= (m_is_four_d ? std::get<3>(m_max) + 1 : 0); ++space) {
		dimension_space next_space;
		bool space_has_active_cube = false;
		for (int layer = std::get<2>(m_min) - 1; layer <= std::get<2>(m_max) + 1; ++layer) {
			dimension_layer next_layer;
			bool layer_has_active_cube = false;
			for (int row = std::get<1>(m_min) - 1; row <= std::get<1>(m_max) + 1; ++row) {
				dimension_row next_row;
				bool row_has_active_cube = false;
				for (int col = std::get<0>(m_min) - 1; col <= std::get<0>(m_max) + 1; ++col) {
					coord next_coord = std::make_tuple(col, row, layer, space);
					cube_state next_state = get_next_cube_state(next_coord);
					next_row.push_back(next_state);

					// If the coordinate is active
					if (next_state == cube_state::ACTIVE) {
						// Update number of active cubes and cache that there is an active cube in each dimension
						++m_active_cubes;
						row_has_active_cube = true;
						layer_has_active_cube = true;
						space_has_active_cube = true;

						// Update the mins and maxes for this dimension
						set_component<0>(&next_min, next_coord, std::min);
						set_component<0>(&next_max, next_coord, std::max);
						set_component<0>(&m_total_min, next_coord, std::min);
						set_component<0>(&m_total_max, next_coord, std::max);
					}
				}

				next_layer.push_back(next_row);
				// If there was an active cube, update the mins and maxes for this dimension
				if (row_has_active_cube) {
					set_component<1>(&next_min, row, std::min);
					set_component<1>(&next_max, row, std::max);
					set_component<1>(&m_total_min, row, std::min);
					set_component<1>(&m_total_max, row, std::max);
				}
			}
						
			next_space.push_back(next_layer);
			// If there was an active cube, update the mins and maxes for this dimension
			if (layer_has_active_cube) {
				set_component<2>(&next_min, layer, std::min);
				set_component<2>(&next_max, layer, std::max);
				set_component<2>(&m_total_min, layer, std::min);
				set_component<2>(&m_total_max, layer, std::max);
			}
		}

		// If there's been an active cube, then we need to add this space
		if (m_active_cubes > 0) {
			next_hyperspace.push_back(next_space);
		}
		// If there was an active cube, update the mins and maxes for this dimension
		if (space_has_active_cube) {
			set_component<3>(&next_min, space, std::min);
			set_component<3>(&next_max, space, std::max);
			set_component<3>(&m_total_min, space, std::min);
			set_component<3>(&m_total_max, space, std::max);
		}
	}

	// Determine the new sizes to know which parts are empty and can be skimmed
	int extra_cols = std::get<0>(next_min) - (std::get<0>(m_min) - 1);
	int new_col_size = std::get<0>(next_max) - std::get<0>(next_min) + 1;
	int extra_rows = std::get<1>(next_min) - (std::get<1>(m_min) - 1);
	int new_row_size = std::get<1>(next_max) - std::get<1>(next_min) + 1;
	int extra_layers = std::get<2>(next_min) - (std::get<2>(m_min) - 1);
	int new_layer_size = std::get<2>(next_max) - std::get<2>(next_min) + 1;
	int new_space_size = std::get<3>(next_max) - std::get<3>(next_min) + 1;

	// Remove hyperspaces at the beginning that were empty
	if (new_space_size < static_cast<int>(next_hyperspace.size())) {
		next_hyperspace.resize(new_space_size);
	}
	for (auto& space : next_hyperspace) {
		// Remove empty layers on bottom
		if (extra_layers > 0) {
			space.erase(space.begin(), space.begin() + extra_layers);
		}
		// Remove empty layers on top
		if (new_layer_size < static_cast<int>(space.size())) {
			space.resize(new_layer_size);
		}

		// Check remaining layers
		for (auto& layer : space) {
			// Remove empty rows on the top
			if (extra_rows > 0) {
				layer.erase(layer.begin(), layer.begin() + extra_rows);
			}
			// Remove empty rows on the bottom
			if (new_row_size < static_cast<int>(layer.size())) {
				layer.resize(new_row_size);
			}

			for (auto& row : layer) {
				// Remove empty coordinates on the left
				if (extra_cols > 0) {
					row.erase(row.begin(), row.begin() + extra_cols);
				}
				// Remove empty coordinates on the right
				if (new_col_size < static_cast<int>(row.size())) {
					row.resize(new_col_size);
				}
			}
		}
	}

	m_hyperspace.swap(next_hyperspace);

	// Reset bbox if there's no active cubes
	if (m_active_cubes == 0) {
		next_min = std::make_tuple(0, 0, 0, 0);
		next_max = std::make_tuple(0, 0, 0, 0);
	}

	m_min = next_min;
	m_max = next_max;

	++m_cycle;
}

// Boot up the cube and go through the initial cycles
void p_dimension::boot_up()
{
	while (!is_boot_done()) {
		next_cycle();
	}
}

// Print the current cube state
void p_dimension::print()
{
	std::cout << "Cycle: " << m_cycle << " Cubes: " << m_active_cubes << "\n\n";
	for (int w = std::get<3>(m_total_min); w <= std::get<3>(m_total_max); ++w) {
		for (int z = std::get<2>(m_total_min); z <= std::get<2>(m_total_max); ++z) {
			std::cout << "Z: " << z << " W: " << w << '\n' << "  ";
			for (int x = std::get<0>(m_total_min) - 1; x <= std::get<0>(m_total_max) + 1; ++x) {
				if (x > -1 && x < 10) {
					std::cout << ' ';
				}
				std::cout << x << ' ';
			}
			std::cout << '\n';
			for (int y = std::get<1>(m_total_min) - 1; y <= std::get<1>(m_total_max) + 1; ++y) {
				if (y > -1 && y < 10) {
					std::cout << ' ';
				}
				std::cout << y << ' ';
				for (int x = std::get<0>(m_total_min) - 1; x <= std::get<0>(m_total_max) + 1; ++x) {
					std::cout << (get_cube_state(coord(x, y, z, w)) == cube_state::ACTIVE ? '@' : '.') << "  ";
				}
				std::cout << '\n';
			}
			std::cout << '\n';
		}
	}
}

/*
* The pocket dimension contains an infinite 3-dimensional grid. 
* At every integer 3-dimensional coordinate (x,y,z), there exists a single cube which is either active or inactive.
* In the initial state of the pocket dimension, almost all cubes start inactive. The only exception to this is a small flat region of cubes (your puzzle input);
* the cubes in this region start in the specified active (#) or inactive (.) state.
* The energy source then proceeds to boot up by executing six cycles.
* Each cube only ever considers its neighbors: any of the 26 other cubes where any of their coordinates differ by at most 1
* During a cycle, all cubes simultaneously change their state according to the following rules:
*   If a cube is active and exactly 2 or 3 of its neighbors are also active, the cube remains active. Otherwise, the cube becomes inactive.
*   If a cube is inactive but exactly 3 of its neighbors are active, the cube becomes active. Otherwise, the cube remains inactive
*/

// How many cubes are left in the active state after the sixth cycle
void problem_1::solve(const std::string& file_name)
{
	std::ifstream input(file_name);
	p_dimension cur_dimension(input);
	input.close();

	cur_dimension.boot_up();

	std::string answer;
	answer = std::to_string(cur_dimension.get_active_cubes_count());
	output_answer(answer);
}

/*
* The pocket dimension actually has four spatial dimensions
* At every integer 4-dimensional coordinate (x,y,z,w), there exists a single cube (really, a hypercube) which is still either active or inactive.
* Each cube only ever considers its neighbors: any of the 80 other cubes where any of their coordinates differ by at most 1
*/

// How many cubes are left in the active state after the sixth cycle
void problem_2::solve(const std::string& file_name)
{
	std::ifstream input(file_name);
	p_dimension cur_dimension(input, true);
	input.close();

	cur_dimension.boot_up();

	std::string answer;
	answer = std::to_string(cur_dimension.get_active_cubes_count());
	output_answer(answer);
}