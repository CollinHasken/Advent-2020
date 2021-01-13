#include "day_22.h"

#include "../common_includes.h"

#include <vector>
#include <array>
#include <set>
#include <algorithm>

PROBLEM_CLASS_CPP(22);

typedef unsigned int player_id;
typedef unsigned int card;
typedef std::vector<card> deck;
typedef std::array<deck, 2> game_state;
typedef std::set<game_state> game_states;

class player {
public:
	player() {};
	player(std::ifstream& input);

	card peek_top_card() { return m_deck.front(); }
	card take_top_card();
	void add_cards(const deck& new_deck);
	void set_winner(bool has_won) { m_is_winner = has_won; }
	void reduce_deck(size_t new_deck_size) { m_deck.resize(new_deck_size); }

	bool is_winner() const { return m_is_winner; }
	const player_id& get_id() const { return m_id; }
	long long int get_score() const;
	size_t get_deck_size() const { return m_deck.size(); }
	const deck& get_deck() const { return m_deck; }

	void print() const;

private:
	player_id m_id = 0;
	bool m_is_winner = false;
	deck m_deck;
};
typedef std::array<player, 2> players;

player::player(std::ifstream& input)
{
	std::string player_id;
	std::getline(input, player_id);
	std::istringstream player_id_stream(player_id);
	player_id_stream.ignore(64, ' ');
	player_id_stream >> m_id;

	std::string card_s;
	std::getline(input, card_s);
	while (!card_s.empty()) {
		m_deck.emplace_back(std::stoi(card_s));
		if (input.eof()) {
			break;
		}
		std::getline(input, card_s);
	}
}

card player::take_top_card()
{
	card top_card = m_deck.front();
	m_deck.erase(m_deck.begin());
	return top_card;
}

void player::add_cards(const deck& new_deck)
{
	m_deck.insert(m_deck.end(), new_deck.begin(), new_deck.end());
}

long long int player::get_score() const
{
	long long int score = 0;
	size_t num_cards = get_deck_size();
	for (size_t cur_card_index = 0; cur_card_index < num_cards; ++cur_card_index) {
		score += static_cast<long long int>(m_deck[cur_card_index]) * (num_cards - cur_card_index);
	}
	return score;
}

void player::print() const
{
	std::cout << "Player " << m_id << "'s deck(" << get_deck_size() << "): ";
	for (size_t card_ind = 0; card_ind < m_deck.size(); ++card_ind) {
		std::cout << m_deck[card_ind];
		if (card_ind != m_deck.size() - 1) {
			std::cout << ',';
		}
		std::cout << ' ';
	}
	std::cout << '\n';
}

class game {
public:
	game(const players& cur_players);
	virtual ~game() {};

	const player& play_game();

protected:
	virtual bool play_round();
	void play_higher_wins(player** winning_player, deck* winning_deck);

	players m_players;
	size_t m_cards_to_win;
};

game::game(const players& cur_players)
{
	m_players = cur_players;
	m_cards_to_win = m_players[0].get_deck_size() + m_players[1].get_deck_size();
}

const player& game::play_game()
{
	while (!play_round()) {
	}

	return m_players[0].is_winner() ? m_players[0] : m_players[1];
}

bool game::play_round()
{
	player* winning_player;
	deck winning_deck;

	play_higher_wins(&winning_player, &winning_deck);

	winning_player->add_cards(winning_deck);
	if(winning_player->get_deck_size() == m_cards_to_win) {
		winning_player->set_winner(true);
		return true;
	}
	return false;
}

void game::play_higher_wins(player** winning_player, deck* winning_deck)
{
	card player_1_card = m_players[0].take_top_card(), player_2_card = m_players[1].take_top_card();
	winning_deck->emplace_back(player_1_card);
	if (player_1_card > player_2_card) {
		*winning_player = &(m_players[0]);
		winning_deck->emplace_back(player_2_card);
	} else {
		*winning_player = &(m_players[1]);
		winning_deck->insert(winning_deck->begin(), player_2_card);
	}	
}

void problem_1::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}
	players cur_players{ player(input), player(input) };
	input.close();
	
	game cur_game(cur_players);
	player winning_player = cur_game.play_game();

	output_answer(std::to_string(winning_player.get_score()));
}

game_states Known_recursions;

class recursive_game : public game {
public:
	recursive_game(const players& cur_players, unsigned int id, bool debug) : game(cur_players), m_id(id), m_debug(debug) { if(m_debug) std::cout << "=== Game " << m_id << " ===\n\n"; };
	~recursive_game() {};

private:
	bool play_round() override;

	bool check_for_recursion();
	void play_new_sub_game(player** winning_player, deck* winning_deck);

	game_states m_game_states;
	bool m_debug;
	unsigned int m_id;
	unsigned int m_round = 0;
};

bool recursive_game::play_round()
{
	player* winning_player;
	deck winning_deck;
	if (m_debug) {
		++m_round;
		std::cout << "-- Round " << m_round << "(Game " << m_id << ") --\n";
		m_players[0].print();
		m_players[1].print();
	}

	if (check_for_recursion()) {
		m_players[0].add_cards(m_players[1].get_deck());
		m_players[0].set_winner(true);
		if (m_debug) {
			std::cin.ignore();
		}
		return true;
	}

	if (m_debug) {
		std::cout << "Player 1 plays: " << m_players[0].peek_top_card() << "\nPlayer 2 plays: " << m_players[1].peek_top_card() << '\n';
	}

	if (m_players[0].peek_top_card() < m_players[0].get_deck_size() && m_players[1].peek_top_card() < m_players[1].get_deck_size()) {
		play_new_sub_game(&winning_player, &winning_deck);
	}	else {
		play_higher_wins(&winning_player, &winning_deck);
	}

	if (m_debug) {
		std::cout << "Player " << winning_player->get_id() << " wins round " << m_round << " of game " << m_id << "\n\n";
	}

	winning_player->add_cards(winning_deck);
	if (winning_player->get_deck_size() == m_cards_to_win) {
		winning_player->set_winner(true);
		if (m_debug) {
			std::cout << "The winner of game " << m_id << " is player " << winning_player->get_id() << "!\n";
		}
		return true;
	}
	return false;
}

bool recursive_game::check_for_recursion()
{
	game_state cur_state{ m_players[0].get_deck(), m_players[1].get_deck() };
	if (Known_recursions.find(cur_state) != Known_recursions.end()) {
		if (m_debug) {
			std::cout << "Matched recursion with round " << std::distance(m_game_states.begin(), m_game_states.find(cur_state)) << "\n The Winner of Game " << m_id << " is Player 1\n";
		}
		return true;
	}
	if (m_game_states.find(cur_state) != m_game_states.end()) {
		if (m_debug) {
			std::cout << "Matched recursion with round " << std::distance(m_game_states.begin(), m_game_states.find(cur_state)) << "\n The Winner of Game " << m_id << " is Player 1\n";
		}
		Known_recursions.insert(m_game_states.begin(), m_game_states.end());
		return true;
	}
	m_game_states.insert(cur_state);
	return false;
}

void recursive_game::play_new_sub_game(player** winning_player, deck* winning_deck)
{
	card player_1_card = m_players[0].take_top_card(), player_2_card = m_players[1].take_top_card();
	winning_deck->emplace_back(player_1_card);
	players sub_players{ m_players[0], m_players[1] };

	sub_players[0].reduce_deck(player_1_card);
	sub_players[1].reduce_deck(player_2_card);

	recursive_game sub_game(sub_players, m_id + 1, m_debug);
	player sub_winning_player = sub_game.play_game();

	if (m_debug) {
		std::cout << "...anyways, back to game " << m_id << '\n';
	}

	if (sub_winning_player.get_id() == m_players[0].get_id()) {
		*winning_player = &(m_players[0]);
		winning_deck->emplace_back(player_2_card);
	} else {
		*winning_player = &(m_players[1]);
		winning_deck->insert(winning_deck->begin(), player_2_card);
	}
}

void problem_2::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}
	players cur_players{ player(input), player(input) };
	input.close();

	recursive_game cur_game(cur_players, 1, false);
	player winning_player = cur_game.play_game();

	output_answer(std::to_string(winning_player.get_score()));
}