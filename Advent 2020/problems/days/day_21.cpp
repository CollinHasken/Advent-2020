#include "day_21.h"

#include "../common_includes.h"

#include <vector>
#include <set>
#include <map>
#include <algorithm>

PROBLEM_CLASS_CPP(21);

typedef std::string id;
typedef std::set<id> ids;

class ingredient {
public:
	ingredient(const id& id) : m_id(id) {};

	void mark_as_known(const id& allergen_id);
	void remove_allergen(const id& known_allergen_id);

	id m_id;
	ids m_allergen_ids;
	int m_count = 1;
	bool m_known_allergen = false;
};

class allergen {
public:
	allergen(const id& allergen_id, const ids& ingredient_ids);

	void add_ingredients(const ids& ingredient_ids);
	void remove_ingredient(const id& ingredient_id);
	void check_for_known_ingredient();

	id m_id;
	ids m_ingredient_ids;
};

typedef std::map<id, ingredient> ingredient_map;
typedef std::map<id, allergen> allergy_map;

allergy_map Allergens;
ingredient_map Ingredients;

void ingredient::remove_allergen(const id& allergen_id)
{
	m_allergen_ids.erase(allergen_id);
}

void ingredient::mark_as_known(const id& known_allergen_id)
{
	m_known_allergen = true;
	for (const id& allergen_id : m_allergen_ids) {
		if (allergen_id.compare(known_allergen_id) == 0) {
			continue;
		}
		Allergens.find(allergen_id)->second.remove_ingredient(m_id);
	}

	m_allergen_ids = ids{ known_allergen_id };
}

allergen::allergen(const id& allergen_id, const ids& ingredient_ids) : m_id(allergen_id)
{
	// Add all non-known ingredients as possible allergens
	for (const std::string& ingredient_id : ingredient_ids) {
		auto& cur_ingredient = Ingredients.find(ingredient_id)->second;

		// New ingredient or not known
		if (!cur_ingredient.m_known_allergen) {
			m_ingredient_ids.emplace(ingredient_id);
			cur_ingredient.m_allergen_ids.emplace(m_id);
		}
	}
	check_for_known_ingredient();
}

void allergen::add_ingredients(const ids& ingredient_ids)
{
	// Make current ingredients that aren't found in the new list remove this as one of their allergens
	for (const id& ingredient_id : m_ingredient_ids) {
		if (ingredient_ids.count(ingredient_id) == 0) {
			Ingredients.find(ingredient_id)->second.remove_allergen(m_id);
		}
	}

	// Remove any current ingredients that aren't found in the new list
	auto it1 = m_ingredient_ids.begin();
	auto it2 = ingredient_ids.begin();
	while ((it1 != m_ingredient_ids.end()) && (it2 != ingredient_ids.end())) {
		if (*it1 < *it2) {
			m_ingredient_ids.erase(it1++);
		} else if (*it2 < *it1) {
			++it2;
		} else { // *it1 == *it2
			++it1;
			++it2;
		}
	}
	// Anything left in m_ingredient_ids from here on did not appear in ingredient_ids so we remove it.
	m_ingredient_ids.erase(it1, m_ingredient_ids.end());

	check_for_known_ingredient();
}

void allergen::remove_ingredient(const id& ingredient_id)
{
	m_ingredient_ids.erase(ingredient_id);
	check_for_known_ingredient();
}

void allergen::check_for_known_ingredient()
{
	if (m_ingredient_ids.size() == 1) {
		Ingredients.find(*m_ingredient_ids.begin())->second.mark_as_known(m_id);
	}
}

static void get_ingredients_and_allergens(std::istringstream* input_stream, ids* ingredients, ids* allergens)
{
	ids* current_ids = ingredients;
	while (!input_stream->eof()) {
		std::string input;
		std::getline(*input_stream, input, ' ');
		if (input[0] == '(') {
			current_ids = allergens;
			continue;
		}
		if (input.back() == ',' || input.back() == ')') {
			input.pop_back();
		}
		current_ids->emplace(input);
	}
}

void problem_1::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	while (!input.eof()) {
		std::string input_line;
		std::getline(input, input_line);
		std::istringstream input_stream(input_line);

		ids ingredient_ids;
		ids allergen_ids;
		get_ingredients_and_allergens(&input_stream, &ingredient_ids, &allergen_ids);

		// Increase count for all ingredients
		for (const std::string& ingredient_id : ingredient_ids) {
			auto ingredients_iter = Ingredients.find(ingredient_id);
			if (ingredients_iter == Ingredients.end()) {
				ingredients_iter = Ingredients.emplace(ingredient_id, ingredient(ingredient_id)).first;
			} else {
				++(ingredients_iter->second.m_count);
			}
		}

		for (const std::string& allergen_id : allergen_ids) {
			auto allergen_iter = Allergens.find(allergen_id);
			if (allergen_iter == Allergens.end()) {
				Allergens.emplace(allergen_id, allergen(allergen_id, ingredient_ids)).first;
			} else {
				// Allergen already existed
				allergen_iter->second.add_ingredients(ingredient_ids);
			}
		}
	}

	input.close();

	int ingredient_count = 0;
	for (const auto& ingredient_pair : Ingredients) {
		if (ingredient_pair.second.m_allergen_ids.empty()) {
			ingredient_count += ingredient_pair.second.m_count;
		}
	}

	std::string answer;
	answer = std::to_string(ingredient_count);
	output_answer(answer);

	Ingredients.clear();
	Allergens.clear();
}

void problem_2::solve(const std::string& file_name)
{
	std::ifstream input(file_name);

	if (!input.is_open()) {
		return;
	}

	while (!input.eof()) {
		std::string input_line;
		std::getline(input, input_line);
		std::istringstream input_stream(input_line);

		ids ingredient_ids;
		ids allergen_ids;
		get_ingredients_and_allergens(&input_stream, &ingredient_ids, &allergen_ids);

		// Increase count for all ingredients
		for (const std::string& ingredient_id : ingredient_ids) {
			auto ingredients_iter = Ingredients.find(ingredient_id);
			if (ingredients_iter == Ingredients.end()) {
				ingredients_iter = Ingredients.emplace(ingredient_id, ingredient(ingredient_id)).first;
			}
			else {
				++(ingredients_iter->second.m_count);
			}
		}

		for (const std::string& allergen_id : allergen_ids) {
			auto allergen_iter = Allergens.find(allergen_id);
			if (allergen_iter == Allergens.end()) {
				Allergens.emplace(allergen_id, allergen(allergen_id, ingredient_ids)).first;
			}
			else {
				// Allergen already existed
				allergen_iter->second.add_ingredients(ingredient_ids);
			}
		}
	}

	input.close();

	std::string ingredient_list;
	for (const auto& cur_allergen : Allergens) {
		ingredient_list.append(*(cur_allergen.second.m_ingredient_ids.begin()));
		ingredient_list.append(",");
	}
	ingredient_list.pop_back();

	output_answer(ingredient_list);

	Ingredients.clear();
	Allergens.clear();
}