#include "day_25.h"

#include "../common_includes.h"

PROBLEM_CLASS_CPP(25);

static constexpr unsigned int LOOP_SUBJECT = 7;
static constexpr unsigned int LOOP_DIVIDOR = 20201227;

// Get the loop size for the two private keys
//
// is_card_loop_size:	(Output) Whether the loop size is for the card or the door
// card_pk:					The card private key
// door_pk:					The door private key
//
// Returns the loops size
int get_loop_size(bool* is_card_loop_size, int card_pk, int door_pk)
{
	int loop_size = 0;
	long long int value = 1;
	while (value != card_pk && value != door_pk) {
		++loop_size;
		//	Set the value to itself multiplied by the subject number
		value *= LOOP_SUBJECT;
		//	Set the value to the remainder after dividing the value by 20201227.
		value %= LOOP_DIVIDOR;
	}
	*is_card_loop_size = value == card_pk;
	return loop_size;
}

// Perfrom the loop to determine the encryption key
//
// subject:		Subject to transform
// loop_size:	Amount of times to loop
//
// Returns the encryption key
long long int perform_loop(int subject, int loop_size)
{
	long long int value = 1;
	for (int i = 0; i < loop_size; ++i) {
		//	Set the value to itself multiplied by the subject number
		value *= subject;
		//	Set the value to the remainder after dividing the value by 20201227.
		value %= LOOP_DIVIDOR;
	}
	return value;
}

/*
* The handshake used by the card and the door involves an operation that transforms a subject number.
* To transform a subject number, start with the value 1. Then, a number of times called the loop size, perform the following steps:
*   Set the value to itself multiplied by the subject number.
*   Set the value to the remainder after dividing the value by 20201227.
* The card always uses a specific, secret loop size when it transforms a subject number. The door always uses a different, secret loop size.
* The cryptographic handshake works like this:
*   The card transforms the subject number of 7 according to the card's secret loop size. The result is called the card's public key.
*   The door transforms the subject number of 7 according to the door's secret loop size. The result is called the door's public key.
*   The card and door use the wireless RFID signal to transmit the two public keys (your puzzle input) to the other device. Now, the card has the door's public key,
*     and the door has the card's public key. Because you can eavesdrop on the signal, you have both public keys, but neither device's loop size.
*   The card transforms the subject number of the door's public key according to the card's loop size. The result is the encryption key.
*   The door transforms the subject number of the card's public key according to the door's loop size. The result is the same encryption key as the card calculated.
* If you can use the two public keys to determine each device's loop size, you will have enough information to calculate the secret encryption key
*/

// What encryption key is the handshake trying to establish
void problem_1::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	// Get private keys
	std::string input_line;
	std::getline(input, input_line);
	int card_pk = std::stoi(input_line);
	
	std::getline(input, input_line);
	int door_pk = std::stoi(input_line);

	input.close();

	// Calculate the loop size
	bool is_card_loop_size = false;
	int loop_size = get_loop_size(&is_card_loop_size, card_pk, door_pk);

	// Perform the loop
	long long int encryption_key = perform_loop(is_card_loop_size ? door_pk : card_pk, loop_size);

	std::string answer;
	answer = std::to_string(encryption_key);
	output_answer(answer);
}

void problem_2::solve(const std::string&)
{
	// No problem 2 for day 25
}