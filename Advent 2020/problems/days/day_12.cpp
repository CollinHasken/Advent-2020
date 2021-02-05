#include "day_12.h"

#include "../common_includes.h"

PROBLEM_CLASS_CPP(12);

# define M_PI           3.14159265358979323846f

typedef std::pair<int, int> coordinate;

enum class cardinal_direction {
	NORTH = 0,
	EAST,
	SOUTH,
	WEST,

	MAX_DIRS,
};

enum class relative_direction {
	LEFT,
	RIGHT,
	FORWARD
};

// Base class for an object on the map with a coordinate and direction
class map_object {
public:
	virtual ~map_object() = 0 {};

	void move(cardinal_direction dir, unsigned int amount);
	virtual void rotate(relative_direction dir, int amount);

	int distance_from_start();
	const coordinate& get_coords() const { return m_coordinate; }

protected:
	coordinate m_coordinate = coordinate(0, 0);
	cardinal_direction m_direction = cardinal_direction::EAST;
};

// Waypoint that the ship moves toward
class waypoint : public map_object{
public:
	waypoint();

	void rotate(relative_direction dir, int amount) override;
};

// Ship to move on the map
class ship : public map_object {
public:
	void move(relative_direction dir, unsigned int amount);
	void move(relative_direction dir, unsigned int amount, const waypoint& waypoint_dir);
};

// Scale operator for coordinate vector
coordinate operator* (const coordinate& lhs, int rhs)
{
	coordinate ans;
	ans.first = lhs.first * rhs;
	ans.second = lhs.second * rhs;
	return ans;
}

static cardinal_direction character_to_direction(char dir_char)
{
	switch (dir_char)
	{
	case 'N':
		return cardinal_direction::NORTH;
	case 'E':
		return cardinal_direction::EAST;
	case 'S':
		return cardinal_direction::SOUTH;
	case 'W':
		return cardinal_direction::WEST;
	default:
		return cardinal_direction::MAX_DIRS;
	}
}

static relative_direction character_to_relative_direction(char dir_char)
{
	switch (dir_char)
	{
	case 'R':
		return relative_direction::RIGHT;
	case 'L':
		return relative_direction::LEFT;
	case 'F':
		return relative_direction::FORWARD;
	default:
		return relative_direction::FORWARD;
	}
}

// Get the relative coordinate for the given cardinal direction
//
// cardinal_direction: Direction to use
//
// Returns the relative coordinate for the direction
static coordinate direction_to_coordinate(cardinal_direction dir)
{
	switch (dir)
	{
	case cardinal_direction::NORTH:
		return coordinate(0, 1);
	case cardinal_direction::EAST:
		return coordinate(1, 0);
	case cardinal_direction::SOUTH:
		return coordinate(0, -1);
	case cardinal_direction::WEST:
		return coordinate(-1, 0);
	default:
		return coordinate(0, 0);
	}
}

// Rotate the map object 
//
// dir:		Direction to turn
// amount:	Amount of times to turn
void map_object::rotate(relative_direction dir, int amount)
{
	int turns = ((amount / 90) % 4) * (dir == relative_direction::RIGHT ? 1 : -1);
	turns = (static_cast<int>(m_direction) + turns) % static_cast<int>(cardinal_direction::MAX_DIRS);
	if (turns < 0) {
		turns += static_cast<int>(cardinal_direction::MAX_DIRS);
	}
	m_direction = static_cast<cardinal_direction>(turns);
}

// Move the map object
// dir:		Direction to move
// amount:	Amount of spaces to move
void map_object::move(cardinal_direction dir, unsigned int amount)
{
	coordinate new_cord = direction_to_coordinate(dir) * amount;
	m_coordinate.first += new_cord.first;
	m_coordinate.second += new_cord.second;
}

// Determine distance from the start
int map_object::distance_from_start()
{
	return (abs(m_coordinate.first) + abs(m_coordinate.second));
}

// Init the waypoint at 10, 1
waypoint::waypoint()
{
	m_coordinate = coordinate(10, 1);
}

// Rotate the waypoint around the ship in the relative direction
//
// dir:		Relative direction to rotate around
// amount:	Degrees to rotate by
void waypoint::rotate(relative_direction dir, int amount)
{
	float angle = (amount / 90) * (.5f * M_PI) * (dir == relative_direction::RIGHT ? -1 : 1);
	float s = sin(angle);
	float c = cos(angle);

	// rotate point
	int x_new = static_cast<int>(round((m_coordinate.first * c) - (m_coordinate.second * s)));
	int y_new = static_cast<int>(round((m_coordinate.first * s) + (m_coordinate.second * c)));

	m_coordinate.first = x_new;
	m_coordinate.second = y_new;
}

// Move the ship
//
// dir:		Direction to move the ship. It should be forward
// amount:	Amount of spaces to move
void ship::move(relative_direction dir, unsigned int amount)
{
	if (dir != relative_direction::FORWARD) {
		return;
	}
	map_object::move(m_direction, amount);
}

// Move the ship towards the waypoint
//
// dir:				Direction to move the ship. It should be forward
// amount:			Amount of times to move the ship to the waypoint
// waypoint_dir:	Direction to waypoint
void ship::move(relative_direction dir, unsigned int amount, const waypoint& waypoint_dir)
{
	if (dir != relative_direction::FORWARD) {
		return;
	}
	coordinate new_cord = waypoint_dir.get_coords() * amount;
	m_coordinate.first += new_cord.first;
	m_coordinate.second += new_cord.second;
}

/*
* A sequence of single-character actions paired with integer input values
* The ship starts by facing east. Only the L and R actions change the direction the ship is facing
* Action N means to move north by the given value.
* Action S means to move south by the given value.
* Action E means to move east by the given value.
* Action W means to move west by the given value.
* Action L means to turn left the given number of degrees.
* Action R means to turn right the given number of degrees.
* Action F means to move forward by the given value in the direction the ship is currently facing.
*/

// What is the Manhattan distance between that location and the ship's starting position
void problem_1::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	ship cur_ship;
	map_object* ship_p = &cur_ship;

	while (!input.eof()) {
		std::string input_line;
		std::getline(input, input_line);
		std::istringstream input_stream(input_line);

		char dir_char = ' ';
		unsigned int amount = 0;
		input_stream >> dir_char >> amount;

		// Move or rotate the ship
		cardinal_direction dir = character_to_direction(dir_char);
		if (dir != cardinal_direction::MAX_DIRS) {
			ship_p->move(dir, amount);
		} else {
			relative_direction rel_dir = character_to_relative_direction(dir_char);
			if (rel_dir == relative_direction::FORWARD) {
				cur_ship.move(rel_dir, amount);
			} else {
				cur_ship.rotate(rel_dir, amount);
			}
		}
	}
	input.close();

	std::string answer;
	answer = std::to_string(cur_ship.distance_from_start());
	output_answer(answer);
}

/*
* Almost all of the actions indicate how to move a waypoint which is relative to the ship's position
* Action N means to move the waypoint north by the given value.
* Action S means to move the waypoint south by the given value.
* Action E means to move the waypoint east by the given value.
* Action W means to move the waypoint west by the given value.
* Action L means to rotate the waypoint around the ship left (counter-clockwise) the given number of degrees.
* Action R means to rotate the waypoint around the ship right (clockwise) the given number of degrees.
* Action F means to move forward to the waypoint a number of times equal to the given value.
* The waypoint starts 10 units east and 1 unit north relative to the ship. The waypoint is relative to the ship; that is, if the ship moves, the waypoint moves with it.
*/

// What is the Manhattan distance between that location and the ship's starting position
void problem_2::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	ship cur_ship;
	waypoint cur_waypoint;
	map_object* ship_p = &cur_ship;
	map_object* waypoint_p = &cur_waypoint;

	while (!input.eof()) {
		std::string input_line;
		std::getline(input, input_line);
		std::istringstream input_stream(input_line);

		char dir_char = ' ';
		unsigned int amount = 0;
		input_stream >> dir_char >> amount;

		// Move the ship or waypoint or rotate the waypoint
		cardinal_direction dir = character_to_direction(dir_char);
		if (dir != cardinal_direction::MAX_DIRS) {
			waypoint_p->move(dir, amount);
		} else {
			relative_direction rel_dir = character_to_relative_direction(dir_char);
			if (rel_dir == relative_direction::FORWARD) {
				cur_ship.move(rel_dir, amount, cur_waypoint);
			} else {
				waypoint_p->rotate(rel_dir, amount);
			}
		}
	}

	input.close();

	std::string answer;
	answer = std::to_string(cur_ship.distance_from_start());
	output_answer(answer);
}