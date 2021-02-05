#include "day_11.h"

#include "../common_includes.h"

#include <vector>
#include <map>

PROBLEM_CLASS_CPP(11);

// Contain the room layout and determine how many seats get filled
class room {
public:
	enum class seating_algorithm {
		ADJACENT,
		VISUAL,
	};

	room(std::ifstream& input, seating_algorithm algo);

	void next_frame();
	bool is_settled() const;
	int count_occupied_seats() const;
	void print() const;

private:
	enum class space {
		CHAIR_OCC,
		CHAIR_EMPTY,
		FLOOR
	};
	typedef room::space(room::* algo_sig) (size_t, size_t);
	typedef std::vector<space> room_layout_row;
	typedef std::vector<room_layout_row> room_layout;

	space get_next_space(size_t row, size_t col);
	room::space do_adjacent(size_t row, size_t col);
	room::space do_visual(size_t row, size_t col);

	std::map<seating_algorithm, algo_sig> m_algos = { {seating_algorithm::ADJACENT, &room::do_adjacent}, {seating_algorithm::VISUAL, &room::do_visual} };

	seating_algorithm m_algo;
	room_layout m_layout;
	bool m_changed_from_last_frame = true;
};

// Initialize a room with input and a certain algorithm
//
// input:	Input to read from
// algo:		Which algorithm to use for the seating
room::room(std::ifstream& input, seating_algorithm algo) :
	m_algo(algo)
{
	while (!input.eof()) {
		std::string input_line;
		std::getline(input, input_line);
		room_layout_row room_line;
		for (const char cur_space : input_line) {
			switch (cur_space)
			{
			case '.':
				room_line.emplace_back(space::FLOOR);
				break;
			case 'L':
				room_line.emplace_back(space::CHAIR_EMPTY);
				break;
			default:
				break;
			}
		}
		m_layout.push_back(room_line);
	}
}

// Do the adjacent algorithm to determine the value for this location for the next round
//
// row:	Row for this space
// col:	Column for this space
room::space room::do_adjacent(size_t row, size_t col)
{
	// Floors don't change
	if (m_layout[row][col] == space::FLOOR) {
		return space::FLOOR;
	}

	const int max_seats = m_layout[row][col] == space::CHAIR_OCC ? 4 : 1;
	int seats_occupied = 0;
	// Check the chairs around us
	for (size_t cur_row = max(0, row - 1); cur_row < min(row + 2, m_layout.size()); ++cur_row) {
		for (size_t cur_col = max(0, col - 1); cur_col < min(col + 2, m_layout[row].size()); ++cur_col) {
			// Don't count the current space
			if (cur_row == row && cur_col == col) {
				continue;
			}
			if (m_layout[cur_row][cur_col] == space::CHAIR_OCC) {
				if (++seats_occupied == max_seats) {
					break;
				}
			}
		}
		if (seats_occupied == max_seats) {
			break;
		}
	}
	return seats_occupied == max_seats ? space::CHAIR_EMPTY : space::CHAIR_OCC;
}

// Do the visual algorithm to determine the value for this location for the next round
//
// row:	Row for this space
// col:	Column for this space
room::space room::do_visual(size_t row, size_t col)
{
	// Floors don't change
	if (m_layout[row][col] == space::FLOOR) {
		return space::FLOOR;
	}

	const int max_seats = m_layout[row][col] == space::CHAIR_OCC ? 5 : 1;
	int seats_occupied = 0;

	// Go through the different x and y scalings to cover each direction from the start
	for (int direction_x_scale = -1; direction_x_scale < 2; ++direction_x_scale) {
		for (int direction_y_scale = -1; direction_y_scale < 2; ++direction_y_scale) {
			// Skip if there will be no changing
			if (direction_x_scale == direction_y_scale && direction_x_scale == 0) {
				continue;
			}

			// Search in the direction for an occupied chair
			for (int cur_row = row + direction_y_scale, cur_col = col + direction_x_scale; (cur_row < static_cast<int>(m_layout.size()) && cur_row >= 0) && (cur_col < static_cast<int>(m_layout[row].size()) && cur_col >= 0); cur_row += direction_y_scale, cur_col += direction_x_scale) {
				if (m_layout[cur_row][cur_col] != space::FLOOR) {
					// If at max seats no need to keep searching
					if (m_layout[cur_row][cur_col] == space::CHAIR_OCC) {
						if (++seats_occupied == max_seats) {
							return seats_occupied == max_seats ? space::CHAIR_EMPTY : space::CHAIR_OCC;
						}
					}
					break;
				}
			}
		}
	}

	return seats_occupied == max_seats ? space::CHAIR_EMPTY : space::CHAIR_OCC;
}

// Get the next space for this position using the room's algorithm
//
// row:	Row for this space
// col:	Column for this space
room::space room::get_next_space(size_t row, size_t col)
{
	return (this->*(m_algos[m_algo]))(row, col);
}

// Compute the layout for the next round
void room::next_frame()
{
	m_changed_from_last_frame = false;
	// Determine the value for each space for the next round
	room_layout next_layout;
	for (size_t row = 0; row < m_layout.size(); ++row) {
		room_layout_row next_room_line;
		for (size_t col = 0; col < m_layout[row].size(); ++col) {
			next_room_line.emplace_back(get_next_space(row, col));
			if (next_room_line[col] != m_layout[row][col]) {
				m_changed_from_last_frame = true;
			}
		}
		next_layout.emplace_back(next_room_line);

	}
	m_layout.swap(next_layout);
}

// Is the room settled
bool room::is_settled() const
{
	return !m_changed_from_last_frame;
}

// Count the number of occupied chairs
//
// Returns the number of occupied chairs
int room::count_occupied_seats() const
{
	int empty_seats = 0;
	for (const room_layout_row& row : m_layout) {
		for (const space cur_space : row) {
			if (cur_space == space::CHAIR_OCC) ++empty_seats;
		}
	}
	return empty_seats;
}

// Print the room layout
void room::print() const
{
	for (const room_layout_row& row : m_layout) {
		for (const space cur_space : row) {
			std::cout << (cur_space == space::CHAIR_EMPTY ? 'L' : (cur_space == space::CHAIR_OCC ? '#' : '.'));
		}
		std::cout << '\n';
	}
}

/*
* The seat layout fits neatly on a grid. Each position is either floor (.), an empty seat (L), or an occupied seat (#)
* All decisions are based on the number of occupied seats adjacent to a given seat (one of the eight positions immediately up, down, left, right, or diagonal from the seat).
*   If a seat is empty (L) and there are no occupied seats adjacent to it, the seat becomes occupied.
*   If a seat is occupied (#) and four or more seats adjacent to it are also occupied, the seat becomes empty.
*   Otherwise, the seat's state does not change.
* Floor (.) never changes; seats don't move, and nobody sits on the floor.
*/

// How many seats end up occupied
void problem_1::solve(const std::string& file_name)
{
	std::ifstream input(file_name);
	room cur_room(input, room::seating_algorithm::ADJACENT);
	input.close();

	std::string a;
	while (!cur_room.is_settled()) {
		cur_room.next_frame();
	}

	std::string answer;
	answer = std::to_string(cur_room.count_occupied_seats());
	output_answer(answer);
}

/*
* People don't just care about adjacent seats - they care about the first seat they can see in each of those eight directions
* It now takes five or more visible occupied seats for an occupied seat to become empty
*/

// How many seats end up occupied
void problem_2::solve(const std::string& file_name)
{
	std::ifstream input(file_name);
	room cur_room(input, room::seating_algorithm::VISUAL);
	input.close();

	std::string a;
	while (!cur_room.is_settled()) {
		cur_room.next_frame();
	}

	std::string answer;
	answer = std::to_string(cur_room.count_occupied_seats());
	output_answer(answer);
}