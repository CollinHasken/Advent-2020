#include "day_20.h"

#include "../common_includes.h"

#include <vector>
#include <map>
#include <array>
#include <algorithm>
#include <regex>

PROBLEM_CLASS_CPP(20);

static constexpr char PIXEL_TYPE_HASH	= '#';
static constexpr char PIXEL_TYPE_DOT	= '.';
static constexpr char PIXEL_TYPE_DRAG	= 'O';
static const std::regex DRAGON_TOP_REGEX(".{18}#.");
static const std::regex DRAGON_MID_REGEX("#(....##){3}#");
static const std::regex DRAGON_BOT_REGEX(".#(..#){5}.{3}");
static constexpr size_t DRAGON_LENGTH = 20;
static constexpr unsigned int DRAGON_HASHES_COUNT = 15;

enum class pixel_type {
	DOT,
	HASH,
};
pixel_type character_to_pixel_type(const char pixel_c) 
{
	return pixel_c == PIXEL_TYPE_HASH ? pixel_type::HASH : pixel_type::DOT;
}
char pixel_type_to_character(pixel_type pixel_t)
{
	return pixel_t == pixel_type::HASH ? PIXEL_TYPE_HASH : PIXEL_TYPE_DOT;
}

// An image's row is a vector of pixels
typedef std::vector<pixel_type> image_row;

// An image is a vector of rows
typedef std::vector<image_row> image;

// The different sides of a the tile
enum class sides {
	TOP = 0,
	BOTTOM,
	RIGHT,
	LEFT,
	TOP_REVERSE,
	BOTTOM_REVERSE,
	RIGHT_REVERSE,
	LEFT_REVERSE,

	NUM_SIDES
};

// Different operations we can use on the tile
enum class operations {
	FLIP_X = 0,
	FLIP_Y,
	ROTATE_RIGHT,
	ROTATE_LEFT
};

// Top, Left to be tested against another tile's Bottom and Right
typedef std::pair<sides, sides> test_edges; 

// Operations required to orient to a direction
typedef std::vector<operations> operations_to_orientation;

// A list of operations to get a pair of edges to be top and left
typedef std::pair<test_edges, operations_to_orientation> test_case;

// Each row contains 9 pixels
// Each pixel can only be one of two types
// Therefore we can convert each row into a unique 9 bit binary representation
typedef short int side_id;
static constexpr side_id INVALID_SIDE_ID = 1 << 10;

// Get the uid for the row where the hashes are 1's
side_id image_row_id(const image_row& row)
{
	side_id id = 0;
	for (auto pixel_t_iter = row.rbegin(); pixel_t_iter != row.rend(); ++pixel_t_iter) {
		id = id << 1;
		if (*pixel_t_iter == pixel_type::HASH) {
			++id;
		}
	}
	return id;
}

// Lookup table
static unsigned char Lookup_reverse[16] = {
0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf, };

// Get the ID of the row that is the reverse of this one
//
// id:	Row to reverse and get ID
//
// Returns the ID of the reversed row
side_id reverse_id(const side_id id)
{
	// Reverse each 4 bits of the 9 bit uid and reverse their order
	return ((Lookup_reverse[id & 0xf] << 8) | (Lookup_reverse[(id >> 4) & 0xf] << 4) | (Lookup_reverse[id >> 8])) >> 2;
}

// The side ids and reverse side ids for a tile
typedef std::array<side_id, 8> side_ids;

// Tile containing it's image, id, and side ids
class tile {
public:
	tile() {};
	tile(std::ifstream& input);
	unsigned int get_id() const { return m_id; }
	const side_ids& get_side_ids() const { return m_side_ids; }

	void rotate_right();
	void rotate_left();
	void flip_x_axis();
	void flip_y_axis();

	void do_operations(const operations_to_orientation& ops);
	void remove_borders();

	unsigned int get_hashes_count() { unsigned int hashes = 0; for (const auto& row : m_image) for (const auto& pixel_t : row) if (pixel_t == pixel_type::HASH) ++hashes; return hashes; }
	size_t get_num_rows() const { return m_image.size(); }
	void get_row_string(std::string* row_s, size_t row) const;
	void print_row(size_t row) const;
	void print() const;
	void print_all() const;

	bool operator==(const tile& rhs) { return m_id == rhs.get_id(); }
protected:
	static constexpr unsigned int TILE_ID_OFFSET = sizeof("Tile") / sizeof(char);

	image m_image;
	unsigned int m_id = -1;
	side_ids m_side_ids = { INVALID_SIDE_ID };
};
// Signitaure for tile operations
typedef void (tile::* operation_sig)(void);

// The different operations that can manipulate the tile
operation_sig Operation_functions[] = {
	&tile::flip_x_axis,
	&tile::flip_y_axis,
	&tile::rotate_right,
	&tile::rotate_left
};

// The test cases for checking if a tile can match with another
std::array<test_case, 8> Test_cases = {
	std::make_pair(std::make_pair(sides::TOP, sides::LEFT), operations_to_orientation{}),
	std::make_pair(std::make_pair(sides::TOP_REVERSE, sides::RIGHT), operations_to_orientation{operations::FLIP_Y}),

	std::make_pair(std::make_pair(sides::BOTTOM, sides::LEFT_REVERSE), operations_to_orientation{operations::FLIP_X}),
	std::make_pair(std::make_pair(sides::BOTTOM_REVERSE, sides::RIGHT_REVERSE), operations_to_orientation{operations::FLIP_X, operations::FLIP_Y}),

	std::make_pair(std::make_pair(sides::RIGHT, sides::TOP_REVERSE), operations_to_orientation{operations::ROTATE_LEFT}),
	std::make_pair(std::make_pair(sides::RIGHT_REVERSE, sides::BOTTOM_REVERSE), operations_to_orientation{operations::ROTATE_LEFT, operations::FLIP_Y}),

	std::make_pair(std::make_pair(sides::LEFT, sides::TOP), operations_to_orientation{operations::ROTATE_RIGHT, operations::FLIP_Y}),	
	std::make_pair(std::make_pair(sides::LEFT_REVERSE, sides::BOTTOM), operations_to_orientation{operations::ROTATE_RIGHT}),
};

// A meta image row is a vector of tiles
typedef std::vector<tile> meta_image_row;

// A meta image is a vector of meta image rows
typedef std::vector<meta_image_row> meta_image;

template <typename T>
using is_vector = std::is_same<T, std::vector<typename T::value_type, typename T::allocator_type>>;
template <typename T, typename R>
using enable_if_vector = typename std::enable_if<is_vector<T>::value, R>::type;

// Flip an array along the x-axis
// 
// cur_array<>:	(Output) The array to be flipped
template<class array_class>
enable_if_vector<array_class, void> flip_x_axis(array_class* cur_array)
{
	std::reverse(cur_array->begin(), cur_array->end());
}

// Flip an array along the y-axis
// 
// cur_array<<>>:	(Output) The array to be flipped
template<class array_class>
enable_if_vector<array_class, void> flip_y_axis(array_class* cur_array)
{
	for (auto& row : *cur_array) {
		std::reverse(row.begin(), row.end());
	}
}

// Rotate an array clockwise
// 
// cur_array<<>>:	(Output) The array to be rotated
template<class array_class>
enable_if_vector<array_class, void> rotate_right(array_class* cur_array)
{
 	array_class temp_array;
	for (size_t old_x = 0; old_x < cur_array->size(); ++old_x) {
		typename array_class::value_type temp_row;
		temp_row.resize(cur_array->size());
		for (size_t old_y = 0; old_y < cur_array->size(); ++old_y) {
			temp_row[old_y] = (*cur_array)[cur_array->size() - 1 - old_y][old_x];
		}
		temp_array.emplace_back(temp_row);
	}
	cur_array->swap(temp_array);
}

// Rotate an array counter-clockwise
// 
// cur_array<<>>:	(Output) The array to be rotated
template<class array_class>
enable_if_vector<array_class, void> rotate_left(array_class* cur_array)
{
	array_class temp_array;
	for (int old_x = cur_array->size() - 1; old_x >= 0; --old_x) {
		typename array_class::value_type temp_row;
		temp_row.resize(cur_array->size());
		for (size_t old_y = 0; old_y < cur_array->size(); ++old_y) {
			temp_row[old_y] = (*cur_array)[old_y][old_x];
		}
		temp_array.emplace_back(temp_row);
	}
	cur_array->swap(temp_array);
}

// Construct a tile from input
tile::tile(std::ifstream& input)
{
	std::string tile_id_s;
	std::getline(input, tile_id_s);

	m_id = std::stoi(tile_id_s.substr(TILE_ID_OFFSET));

	// Cache the edges
	image_row top, bottom, left, right;

	std::string image_row_s;
	std::getline(input, image_row_s);
	image_row new_row;
	// Extract all the rows
	while (!image_row_s.empty()) {
		new_row.clear();
		for (const char image_pixel : image_row_s) {
			pixel_type pixel_t = character_to_pixel_type(image_pixel);
			if (new_row.empty()) {
				left.push_back(pixel_t);
			}
			new_row.push_back(pixel_t);
		}
		right.push_back(new_row.back());
		m_image.push_back(new_row);
		if (m_image.size() == 1) {
			top.swap(new_row);
		}

		if (input.eof()) {
			break;
		}
		std::getline(input, image_row_s);
	}
	bottom.swap(new_row);

	// Cache the side ids
	m_side_ids[static_cast<int>(sides::TOP)] = image_row_id(top);
	m_side_ids[static_cast<int>(sides::BOTTOM)] = image_row_id(bottom);
	m_side_ids[static_cast<int>(sides::RIGHT)] = image_row_id(right);
	m_side_ids[static_cast<int>(sides::LEFT)] = image_row_id(left);
	m_side_ids[static_cast<int>(sides::TOP_REVERSE)] = reverse_id(m_side_ids[static_cast<int>(sides::TOP)]);
	m_side_ids[static_cast<int>(sides::BOTTOM_REVERSE)] = reverse_id(m_side_ids[static_cast<int>(sides::BOTTOM)]);
	m_side_ids[static_cast<int>(sides::RIGHT_REVERSE)] = reverse_id(m_side_ids[static_cast<int>(sides::RIGHT)]);
	m_side_ids[static_cast<int>(sides::LEFT_REVERSE)] = reverse_id(m_side_ids[static_cast<int>(sides::LEFT)]);
}

// Get the string for the given row
//
// row_s:	(Output) The string made by the row
// row:		The row index
void tile::get_row_string(std::string* row_s, size_t row) const
{
	for (pixel_type pixel : m_image[row]) {
		row_s->append(1, pixel_type_to_character(pixel));
	}
}

// Print the row
//
// row:	Row index to print
void tile::print_row(size_t row) const
{
	std::string row_s;
	get_row_string(&row_s, row);
	std::cout << row_s;
}

// Print the tile
void tile::print() const
{
	for (size_t row = 0; row < get_num_rows(); ++row) {
		print_row(row);
		std::cout << '\n';
	}
	std::cout << '\n';
}

// Print the tile and its info
void tile::print_all() const
{
	std::cout << "ID: " << m_id << '\n';
	print();
	std::cout << "Side IDs:" << '\n';
	std::cout << '\t' << m_side_ids[static_cast<int>(sides::TOP)] << "\t\t\t" << m_side_ids[static_cast<int>(sides::TOP_REVERSE)] << '\n';
	std::cout << m_side_ids[static_cast<int>(sides::LEFT)] << "\t\t" << m_side_ids[static_cast<int>(sides::RIGHT)] << '\t' << m_side_ids[static_cast<int>(sides::LEFT_REVERSE)] << "\t\t" << m_side_ids[static_cast<int>(sides::RIGHT_REVERSE)] << '\n';
	std::cout << '\t' << m_side_ids[static_cast<int>(sides::BOTTOM)] << "\t\t\t" << m_side_ids[static_cast<int>(sides::BOTTOM_REVERSE)] << '\n';
}

// Rotate the tile clockwise
void tile::rotate_right()
{
	// Do the generic rotate
	::rotate_right(&m_image);

	// Make T be on the right and R on the top
	std::swap(m_side_ids[static_cast<int>(sides::TOP)], m_side_ids[static_cast<int>(sides::RIGHT)]);
	std::swap(m_side_ids[static_cast<int>(sides::TOP_REVERSE)], m_side_ids[static_cast<int>(sides::RIGHT_REVERSE)]);
	
	// Make Reverse L be on the top and Reverse R on the left
	std::swap(m_side_ids[static_cast<int>(sides::LEFT)], m_side_ids[static_cast<int>(sides::TOP_REVERSE)]);
	std::swap(m_side_ids[static_cast<int>(sides::LEFT_REVERSE)], m_side_ids[static_cast<int>(sides::TOP)]);

	// make B be on the left, and Reverse R be on the bottom
	std::swap(m_side_ids[static_cast<int>(sides::BOTTOM)], m_side_ids[static_cast<int>(sides::LEFT)]); 
	std::swap(m_side_ids[static_cast<int>(sides::BOTTOM_REVERSE)], m_side_ids[static_cast<int>(sides::LEFT_REVERSE)]);
}

// Rotate the tile counter-clockwise
void tile::rotate_left()
{
	// Do the generic rotate
	::rotate_left(&m_image);

	// Make B be on the left and L on the bottom
	std::swap(m_side_ids[static_cast<int>(sides::BOTTOM)], m_side_ids[static_cast<int>(sides::LEFT)]);
	std::swap(m_side_ids[static_cast<int>(sides::BOTTOM_REVERSE)], m_side_ids[static_cast<int>(sides::LEFT_REVERSE)]);

	// Make Reverse B be on the top and Reverse T on the left
	std::swap(m_side_ids[static_cast<int>(sides::LEFT)], m_side_ids[static_cast<int>(sides::TOP_REVERSE)]);
	std::swap(m_side_ids[static_cast<int>(sides::LEFT_REVERSE)], m_side_ids[static_cast<int>(sides::TOP)]);

	// make Reverse B be on the right, and R be on the top
	std::swap(m_side_ids[static_cast<int>(sides::TOP)], m_side_ids[static_cast<int>(sides::RIGHT)]);
	std::swap(m_side_ids[static_cast<int>(sides::TOP_REVERSE)], m_side_ids[static_cast<int>(sides::RIGHT_REVERSE)]);
}

// Flip the tile along the x axis
void tile::flip_x_axis()
{
	// Do the generic flip
	::flip_x_axis(&m_image);

	// Make T be on bottom and B on top
	std::swap(m_side_ids[static_cast<int>(sides::TOP)], m_side_ids[static_cast<int>(sides::BOTTOM)]);
	std::swap(m_side_ids[static_cast<int>(sides::TOP_REVERSE)], m_side_ids[static_cast<int>(sides::BOTTOM_REVERSE)]);

	// Make Reverse L be on the left and Reverse R on the right
	std::swap(m_side_ids[static_cast<int>(sides::LEFT_REVERSE)], m_side_ids[static_cast<int>(sides::LEFT)]);
	std::swap(m_side_ids[static_cast<int>(sides::RIGHT_REVERSE)], m_side_ids[static_cast<int>(sides::RIGHT)]);
}

// Flip the tile along the y axis
void tile::flip_y_axis()
{
	// Do the generic flip
	::flip_y_axis(&m_image);

	// Make L be on the right and R on the left
	std::swap(m_side_ids[static_cast<int>(sides::LEFT)], m_side_ids[static_cast<int>(sides::RIGHT)]);
	std::swap(m_side_ids[static_cast<int>(sides::LEFT_REVERSE)], m_side_ids[static_cast<int>(sides::RIGHT_REVERSE)]);

	// Make Reverse T be on the top and Reverse B on the bottom
	std::swap(m_side_ids[static_cast<int>(sides::TOP_REVERSE)], m_side_ids[static_cast<int>(sides::TOP)]);
	std::swap(m_side_ids[static_cast<int>(sides::BOTTOM_REVERSE)], m_side_ids[static_cast<int>(sides::BOTTOM)]);
}

// Do the corresponding operations to change the orientation
//
// ops:	The operations to perform
void tile::do_operations(const operations_to_orientation& ops)
{
	for (const operations op : ops) {
		(this->*Operation_functions[static_cast<int>(op)])();
	}
}

// Remove the borders on the tile
void tile::remove_borders()
{
	// Make the first and last row the last two elements
	std::rotate(m_image.begin(), m_image.begin() + 1, m_image.end());
	m_image.resize(m_image.size() - 2);

	// Rotate each row's vector and get rid of the two on the end as they were the first and last
	for (image_row& row : m_image) {
		std::rotate(row.begin(), row.begin() + 1, row.end());
		row.resize(row.size() - 2);
	}
}


// Print the meta image
//
// cur_meta_image:	Meta image to print
// all:					(Optional) Whether to print all info
void print_meta_image(const meta_image& cur_meta_image, bool all = false)
{
	if (cur_meta_image.empty() || cur_meta_image.front().empty()) {
		std::cout << "EMPTY\n";
		return;
	}

	size_t rows = cur_meta_image.front().front().get_num_rows();
	for (const meta_image_row& cur_row : cur_meta_image) {
		if (all) {
			for (const tile& cur_tile : cur_row) {
				std::cout << "   " << cur_tile.get_id() << "\t\t";
			}
			std::cout << '\n';
		}

		for (size_t row = 0; row < rows; ++row) {
			for (const tile& cur_tile : cur_row) {
				cur_tile.print_row(row);
				if (all) {
					std::cout << '\t';
				}
			}
			std::cout << '\n';
		}
		if (all) {
			std::cout << '\n';
		}
	}

	std::cout << std::string(16 * cur_meta_image.front().size(), '-') << '\n';
}

// Fill the meta image with the tiles in a given direction
//
// cur_meta_image:			(Output) Meta image to fill
// remaining_tiles:			(Output) The tiles to try from. Everything les than the index has been used
// reamining_tile_index:	(Output) The index from which tiles have not been used yet
// start_y:						The y value start at
// add_direction:				What direction to add to
// width:						The width to fill to
void fill_out(meta_image* cur_meta_image, std::vector<tile>* remaining_tiles, size_t* remaining_tile_index, const int start_y, const sides add_direction, const int width) {
	// Keep searching until we're at the edge
	const tile* cur_tile_p = &(cur_meta_image->back().front());
	int cur_x = 0;
	int cur_y = start_y;
	tile* found_tile_p;
	do {
		// Find the next tile
		found_tile_p = nullptr;
		const tile* cur_left_tile_p = cur_x == 0 ? nullptr : &((*cur_meta_image)[cur_y][cur_x - 1]);
		for (size_t test_tile_ind = *remaining_tile_index; test_tile_ind < remaining_tiles->size(); ++test_tile_ind) {
			tile* test_tile_p = &(*remaining_tiles)[test_tile_ind];

			// Test if there's an operation to orient the test tile to have it's edge match the current's edge matching the add direction
			for (const test_case& test : Test_cases) {
				int test_side = static_cast<int>((add_direction == sides::TOP || add_direction == sides::BOTTOM) ? test.first.first : test.first.second);
				// The tests are made to test against the current tile's bottom and right edge
				// If the current tile's edge we're testing isn't one of those, then we need to get the opposite side to test with
				// The edge pairs only differ by the last bit
				if (add_direction != sides::BOTTOM && add_direction != sides::RIGHT) {
					test_side ^= 1;
				}

				if (cur_tile_p->get_side_ids()[static_cast<int>(add_direction)] != test_tile_p->get_side_ids()[test_side]) {
					continue;
				}

				// If the current row is full and we should be going in the y direction, then we need to add a new row
				switch (add_direction)
				{
				case sides::TOP:
				case sides::BOTTOM:
					if (cur_left_tile_p == nullptr) {
						cur_meta_image->emplace(std::next(cur_meta_image->begin(), cur_y), meta_image_row{ *test_tile_p });
					} else {
						// Otherwise we also need to test the left neighbor
						if (cur_left_tile_p->get_side_ids()[static_cast<int>(sides::RIGHT)] != test_tile_p->get_side_ids()[static_cast<int>(test.first.second)]) {
							continue;
						}

						// Then add to current row
						(*cur_meta_image)[cur_y].emplace_back(*test_tile_p);
					}
					found_tile_p = &((*cur_meta_image)[cur_y][cur_x]);
					++cur_x;
					break;
				case sides::RIGHT:
					cur_meta_image->back().emplace_back(*test_tile_p);
					++cur_x;
					found_tile_p = &((*cur_meta_image)[cur_y][cur_x]);
					break;
				case sides::LEFT:
					cur_meta_image->back().emplace(cur_meta_image->back().begin(), *test_tile_p);
					found_tile_p = &((*cur_meta_image)[cur_y][cur_x]);
					break;
				default:
					break;
				}

				// Orient the test tile to match
				found_tile_p->do_operations(test.second);

				// Move the used tile to the front of the remaining tiles
				std::swap((*remaining_tiles)[(*remaining_tile_index)++], (*remaining_tiles)[test_tile_ind]);

				// If there's no right neighbor, then the next tile will be the first one in the new row
				if (cur_x == width) {
					cur_tile_p = &((*cur_meta_image)[cur_y].front());
					cur_x = 0;
					cur_y += add_direction == sides::TOP ? 0 : 1;
				} else {
					// Otherwise, get the next tile
					cur_tile_p = &((*cur_meta_image)[cur_y + (add_direction == sides::TOP ? 1 : add_direction == sides::BOTTOM ? -1 : 0)][cur_x]);
				}
				break;
			}
			// Found, stop searching
			if (found_tile_p != nullptr) {
				break;
			}
		}
	} while (found_tile_p != nullptr);
}

// Create the meta image from the given tiles
//
// cur_meta_image:	(Output) The made meta image
// remaining_tiels:	The tiles left to add
void create_meta_image(meta_image* cur_meta_image, std::vector<tile>* remaining_tiles)
{
	// No tiles remaining
	if (remaining_tiles->empty()) {
		return;
	}

	const int width = static_cast<int>(std::sqrt(remaining_tiles->size()));
	size_t cur_tile_index = 0;
	tile* cur_tile_p = &((*remaining_tiles)[cur_tile_index++]);
	cur_meta_image->emplace_back(meta_image_row{ *cur_tile_p });	

	// Fill the meta image starting from a random tile and then filling the rest of the right side of the row
	fill_out(cur_meta_image, remaining_tiles, &cur_tile_index, 0, sides::RIGHT, width);
	// Then filling the left side of the row from the start
	fill_out(cur_meta_image, remaining_tiles, &cur_tile_index, 0, sides::LEFT, width);
	// Then fill the rows above this one
	fill_out(cur_meta_image, remaining_tiles, &cur_tile_index, 0, sides::TOP, width);
	// Then fill the rows below the start
	fill_out(cur_meta_image, remaining_tiles, &cur_tile_index, cur_meta_image->size(), sides::BOTTOM, width);
}

/*
* The data actually contains many small images created by the satellite's camera array.
* The camera array consists of many cameras; rather than produce a single square image, they produce many smaller square image tiles that need to be reassembled back into a single image.
* Each camera in the camera array returns a single monochrome image tile with a random unique ID number. The tiles (your puzzle input) arrived in a random order
* Each image tile has been rotated and flipped to a random orientation. Your first task is to reassemble the original image by orienting the tiles so they fit together
* ach tile's image data includes a border that should line up exactly with its adjacent tiles. All tiles have this border, and the border lines up exactly when the tiles are both oriented correctly.
* Tiles at the edge of the image also have this border, but the outermost edges won't line up with any other tiles
*/

// What do you get if you multiply together the IDs of the four corner tiles
void problem_1::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	// Get all the tiles
	std::vector<tile> tiles;
	while (!input.eof()) {
		tiles.emplace_back(tile(input));
	}
	input.close();

	// Create the meta image from the tiles
	meta_image cur_meta_image;
	create_meta_image(&cur_meta_image, &tiles);

	// Multiply the four corners ids
	long long int corners = 1;
	corners *= cur_meta_image.front().front().get_id();
	corners *= cur_meta_image.front().back().get_id();
	corners *= cur_meta_image.back().front().get_id();
	corners *= cur_meta_image.back().back().get_id();

	std::string answer;
	answer = std::to_string(corners);
	output_answer(answer); 
}

// Add a dragon to a string representation of the meta image
//
// meta_image_s:			(Output) The meta image string to put the dragon in
// row_length:				The length of each row in pixels
// string_row:				The row the first dragon line is on in the array
// offset_from_newline:	The offset from the previous new line
void add_dragon(std::string* meta_image_s, size_t row_length, size_t string_row, size_t offset_from_newline)
{
	(*meta_image_s)[((row_length + 1) * (string_row - 1)) + offset_from_newline + 18] = 'O';

	size_t mid_offset = ((row_length + 1) * (string_row)) + offset_from_newline;
	(*meta_image_s)[mid_offset] = PIXEL_TYPE_DRAG;
	(*meta_image_s)[mid_offset + 5] = PIXEL_TYPE_DRAG;
	(*meta_image_s)[mid_offset + 6] = PIXEL_TYPE_DRAG;
	(*meta_image_s)[mid_offset + 11] = PIXEL_TYPE_DRAG;
	(*meta_image_s)[mid_offset + 12] = PIXEL_TYPE_DRAG;
	(*meta_image_s)[mid_offset + 17] = PIXEL_TYPE_DRAG;
	(*meta_image_s)[mid_offset + 18] = PIXEL_TYPE_DRAG;
	(*meta_image_s)[mid_offset + 19] = PIXEL_TYPE_DRAG;

	size_t bot_offset = ((row_length + 1) * (string_row + 1)) + offset_from_newline;
	(*meta_image_s)[bot_offset + 1] = PIXEL_TYPE_DRAG;
	(*meta_image_s)[bot_offset + 4] = PIXEL_TYPE_DRAG;
	(*meta_image_s)[bot_offset + 7] = PIXEL_TYPE_DRAG;
	(*meta_image_s)[bot_offset + 10] = PIXEL_TYPE_DRAG;
	(*meta_image_s)[bot_offset + 13] = PIXEL_TYPE_DRAG;
	(*meta_image_s)[bot_offset + 16] = PIXEL_TYPE_DRAG;
}

// Get the meta image as a string
//
// meta_image_s:			(Output) The meta image string
// meta_image_strings:	The rows of the meta image in string form
void get_meta_image_string(std::string* meta_image_s, const std::vector<std::string>& meta_image_strings)
{
	for (const std::string& row_s : meta_image_strings) {
		meta_image_s->append(row_s);
		meta_image_s->append(1, '\n');
	}
}

// Count the amount of dragons in the image
//
// cur_meta_image:	Image to search
// 
// Return the number of dragons found
unsigned int get_dragons_count(meta_image* cur_meta_image)
{
	// Operations to test every orientation in succession
	static const operations_to_orientation ops = {
		operations::FLIP_Y,
		operations::FLIP_X,
		operations::FLIP_Y,
		operations::ROTATE_RIGHT,
		operations::FLIP_Y,
		operations::FLIP_X,
		operations::FLIP_Y,
	};

	// Operation to function
	static const std::map<operations, void(*)(std::vector<std::string>*)> ops_to_funcs = {
		{ operations::FLIP_X, flip_x_axis<std::vector<std::string>> },
		{ operations::FLIP_Y, flip_y_axis<std::vector<std::string>> },
		{ operations::ROTATE_RIGHT, rotate_right<std::vector<std::string>> },
	};

	// Convert the meta image into a vector of each row as a string
	std::vector<std::string> meta_image_strings;
	for (const meta_image_row& row : *cur_meta_image) {
		for (size_t row_ind = 0; row_ind < row.front().get_num_rows(); ++row_ind) {
			std::string new_row;
			for (const tile& cur_tile : row) {
				cur_tile.get_row_string(&new_row, row_ind);
			}
			meta_image_strings.emplace_back(new_row);
		}
	}

	const size_t row_length = meta_image_strings.front().size();
	size_t cur_op = 0;
	unsigned int num_dragons = 0;
	std::string meta_image_string;
	while (num_dragons == 0) {
		// Update string
		meta_image_string.clear();
		get_meta_image_string(&meta_image_string, meta_image_strings);

		// Find match of the mid string as its the hardest to match
		size_t search_offset = 0;
		std::string search_string = meta_image_string;
		std::smatch dragon_match;
		while (std::regex_search(search_string, dragon_match, DRAGON_MID_REGEX)) {
			// Get the next line's string starting from where the dragon started
			search_offset += dragon_match.position();
			size_t string_row = search_offset / (meta_image_strings.front().size() + 1);
			if (string_row == meta_image_strings.size() - 1) {
				break;
			}
			if (string_row == 0) {
				continue;
			}

			size_t offset_from_newline = search_offset - meta_image_string.find_last_of('\n', search_offset) - 1;
			std::string test_string = meta_image_strings[string_row + 1].substr(offset_from_newline, DRAGON_LENGTH);

			// Test if the bottom string matches
			if (std::regex_match(test_string, DRAGON_BOT_REGEX)) {
				// Get the previous line's string starting from where the dragon started
				test_string = meta_image_strings[string_row - 1].substr(offset_from_newline, DRAGON_LENGTH);

				// Test if the bottom string matches			
				if (std::regex_match(test_string, DRAGON_TOP_REGEX)) {
					++num_dragons;
					add_dragon(&meta_image_string, row_length, string_row, offset_from_newline);
				}
			}

			// Move on to the next possible position
			search_string = meta_image_string.substr(++search_offset);
		}

		// No dragons found, new orientation
		if (num_dragons == 0) {
			if (cur_op == ops.size()) {
				break;
			}
			(*ops_to_funcs.find(ops[cur_op++])->second)(&meta_image_strings);
		}
	}
	std::cout << meta_image_string << '\n';
	return num_dragons;
}

/*
* Now, you're ready to check the image for sea monsters. The borders of each tile are not part of the actual image; start by removing them
* a sea monster will look like this:
*                   # 
* #    ##    ##    ###
*  #  #  #  #  #  #   
* The spaces can be anything; only the # need to match. Also, you might need to rotate or flip your image before it's oriented correctly to find sea monsters
*/

// How many # are not part of a sea monster
void problem_2::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	// Get all the tiles
	std::vector<tile> tiles;
	while (!input.eof()) {
		tiles.emplace_back(tile(input));
	}

	input.close();

	// Create the meta image from the tiles
	meta_image cur_meta_image;
	create_meta_image(&cur_meta_image, &tiles);

	// Remove borders and get the total amount of hashes
	int hash_num = 0;
	for (meta_image_row& row : cur_meta_image) {
		for (tile& cur_tile : row) {
			cur_tile.remove_borders();
			hash_num += cur_tile.get_hashes_count();
		}
	}

	// Remove the hashes used by the dragons
	hash_num -= get_dragons_count(&cur_meta_image) * DRAGON_HASHES_COUNT;

	std::string answer;
	answer = std::to_string(hash_num);
	output_answer(answer);
}