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

// Player that has their own deck
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

// Construct player from input
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

// Take the top card from their deck and remove it
//
// Returns the top card
card player::take_top_card()
{
	card top_card = m_deck.front();
	m_deck.erase(m_deck.begin());
	return top_card;
}

// Add cards to the player's deck
//
// new_deck:	Deck to add
void player::add_cards(const deck& new_deck)
{
	m_deck.insert(m_deck.end(), new_deck.begin(), new_deck.end());
}

// Get the player's score
long long int player::get_score() const
{
	long long int score = 0;
	size_t num_cards = get_deck_size();
	for (size_t cur_card_index = 0; cur_card_index < num_cards; ++cur_card_index) {
		score += static_cast<long long int>(m_deck[cur_card_index]) * (num_cards - cur_card_index);
	}
	return score;
}

// Print the player's deck
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

// Game that is played by players
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

// Initialize the game with players that have starting decks
game::game(const players& cur_players)
{
	m_players = cur_players;
	m_cards_to_win = m_players[0].get_deck_size() + m_players[1].get_deck_size();
}

// Play the game
//
// Returns the player that won
const player& game::play_game()
{
	while (!play_round()) {
	}

	return m_players[0].is_winner() ? m_players[0] : m_players[1];
}

// Play the next round
//
// Returns true if someone has won
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

// Play higher wins round
//
// winning_player:	(Output) The player that won
// winning_deck:		(Output) The deck the winning player should receive
void game::play_higher_wins(player** winning_player, deck* winning_deck)
{
	card player_1_card = m_players[0].take_top_card(), player_2_card = m_players[1].take_top_card();
	winning_deck->emplace_back(player_1_card);

	// If player 1 won, add player 2's card to the back
	if (player_1_card > player_2_card) {
		*winning_player = &(m_players[0]);
		winning_deck->emplace_back(player_2_card);
	} else {
		// If player 2 won, then add player 2's card to the front
		*winning_player = &(m_players[1]);
		winning_deck->insert(winning_deck->begin(), player_2_card);
	}	
}

/*
* Before the game starts, split the cards so each player has their own deck (your puzzle input).
* Then, the game consists of a series of rounds: both players draw their top card, and the player with the higher-valued card wins the round.
* The winner keeps both cards, placing them on the bottom of their own deck so that the winner's card is above the other card.
* If this causes a player to have all of the cards, they win, and the game ends
* The bottom card in their deck is worth the value of the card multiplied by 1,
* the second-from-the-bottom card is worth the value of the card multiplied by 2, and so on.
* With 10 cards, the top card is worth the value on the card multiplied by 10
*/

// What is the winning player's score
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

// States of the game that are known to recurse
game_states Known_recursions;

// A game that includes recursion
class recursive_game : public game {
public:
	recursive_game(const players& cur_players, unsigned int id) : game(cur_players), m_id(id) {};
	~recursive_game() {};

private:
	bool play_round() override;

	bool check_for_recursion();
	void play_new_sub_game(player** winning_player, deck* winning_deck);

	game_states m_game_states;
	unsigned int m_id;
	unsigned int m_round = 0;
};

// Play round of the game
//
// Returns true if someone won
bool recursive_game::play_round()
{
	player* winning_player;
	deck winning_deck;

	// If this is recursion
	if (check_for_recursion()) {
		// Give player 1 the win
		m_players[0].add_cards(m_players[1].get_deck());
		m_players[0].set_winner(true);
		return true;
	}

	// Determine which game to play
	if (m_players[0].peek_top_card() < m_players[0].get_deck_size() && m_players[1].peek_top_card() < m_players[1].get_deck_size()) {
		play_new_sub_game(&winning_player, &winning_deck);
	}	else {
		play_higher_wins(&winning_player, &winning_deck);
	}

	winning_player->add_cards(winning_deck);
	// If someone has won
	if (winning_player->get_deck_size() == m_cards_to_win) {
		winning_player->set_winner(true);		
		return true;
	}
	return false;
}

// Check if this state is known as a recursion point
bool recursive_game::check_for_recursion()
{
	game_state cur_state{ m_players[0].get_deck(), m_players[1].get_deck() };
	// It's known recursion
	if (Known_recursions.find(cur_state) != Known_recursions.end()) {
		return true;
	}
	// If this state has happened before in this game
	if (m_game_states.find(cur_state) != m_game_states.end()) {
		// Add all states up to known recursions as they'll all end up here
		Known_recursions.insert(m_game_states.begin(), m_game_states.end());
		return true;
	}
	m_game_states.insert(cur_state);
	return false;
}

// Play a sub game
//
// winning_player:	(Output) The player that won
// winning_deck:		(Output) The deck the winning player should receive
void recursive_game::play_new_sub_game(player** winning_player, deck* winning_deck)
{
	card player_1_card = m_players[0].take_top_card(), player_2_card = m_players[1].take_top_card();
	winning_deck->emplace_back(player_1_card);
	// Create new players to play the sub game
	players sub_players{ m_players[0], m_players[1] };

	// Reduce their decks to their new size
	sub_players[0].reduce_deck(player_1_card);
	sub_players[1].reduce_deck(player_2_card);

	// Play the game
	recursive_game sub_game(sub_players, m_id + 1);
	player sub_winning_player = sub_game.play_game();

	// Assign winner
	if (sub_winning_player.get_id() == m_players[0].get_id()) {
		*winning_player = &(m_players[0]);
		winning_deck->emplace_back(player_2_card);
	} else {
		*winning_player = &(m_players[1]);
		winning_deck->insert(winning_deck->begin(), player_2_card);
	}
}

/*
* Recursive Combat still starts by splitting the cards into two decks (you offer to play with the same starting decks as before - it's only fair). Then, the game consists of a series of rounds with a few changes:
*   Before either player deals a card, if there was a previous round in this game that had exactly the same cards in the same order in the same players' decks, the game instantly ends in a win for player 1.
*   Previous rounds from other games are not considered. (This prevents infinite games of Recursive Combat, which everyone agrees is a bad idea.)
*   Otherwise, this round's cards must be in a new configuration; the players begin the round by each drawing the top card of their deck as normal.
*   If both players have at least as many cards remaining in their deck as the value of the card they just drew, the winner of the round is determined by playing a new game of Recursive Combat (see below).
*   Otherwise, at least one player must not have enough cards left in their deck to recurse; the winner of the round is the player with the higher-value card.
* As in regular Combat, the winner of the round (even if they won the round by winning a sub-game)
* takes the two cards dealt at the beginning of the round and places them on the bottom of their own deck (again so that the winner's card is above the other card).
* Note that the winner's card might be the lower-valued of the two cards if they won the round due to winning a sub-game.
* If collecting cards by winning the round causes a player to have all of the cards, they win, and the game ends
* During a round of Recursive Combat, if both players have at least as many cards in their own decks as the number on the card they just dealt,
* the winner of the round is determined by recursing into a sub-game of Recursive Combat
* To play a sub-game of Recursive Combat, each player creates a new deck by making a copy of the next cards in their deck 
* (the quantity of cards copied is equal to the number on the card they drew to trigger the sub-game).
* During this sub-game, the game that triggered it is on hold and completely unaffected; no cards are removed from players' decks to form the sub-game
* After the game, the winning player's score is calculated from the cards they have in their original deck using the same rules as regular Combat
*/

// What is the winning player's score
void problem_2::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}
	players cur_players{ player(input), player(input) };
	input.close();

	recursive_game cur_game(cur_players, 1);
	player winning_player = cur_game.play_game();

	output_answer(std::to_string(winning_player.get_score()));
}